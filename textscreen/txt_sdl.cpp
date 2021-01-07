/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Text mode emulation in SDL
\**********************************************************************************************************************************************/

#include "../derma/d_native.h"

#include "SDL.h"

#include "txt_sdl.h"

#include "doomkeys.h"

#include "txt_main.h"
#include "txt_utf8.h"

#include "fonts/small.h"
#include "fonts/normal.h"
#include "fonts/large.h"
#include "fonts/codepage.h"

namespace cudadoom::txt
{

// Time between character blinks in ms
constexpr int BLINK_PERIOD{250};
constexpr int LARGE_FONT_THRESHOLD{144};

SDL_Window* SDLWindow;
static SDL_Surface* screenbuffer;
static unsigned char* screendata;
static SDL_Renderer* renderer;

// Current input mode.
static InputType input_mode{InputType::normal};

// Dimensions of the screen image in screen coordinates (not pixels); this is the value that was passed to SDL_CreateWindow().
static int screen_image_w;
static int screen_image_h;

static TxtSDLEventCallbackFunc event_callback;
static void* event_callback_data;

// Font we are using:
static const FontType* font;

// Dummy "font" that means to try highdpi rendering, or fallback to
// normal_font otherwise.
constexpr FontType highdpi_font{"normal-highdpi", nullptr, 8, 16};

// Mapping from SDL keyboard scancode to internal key code.
//static const int scancode_translate_table[]{SCANCODE_TO_KEYS_ARRAY};

// String names of keys. This is a fallback; we usually use the SDL API.
//static const struct
//{
	//int key;
	//std::string name;
//} key_names[]{KEY_NAMES_ARRAY};

// Unicode key mapping; see codepage.h.
static const short code_page_to_unicode[]{CODE_PAGE_TO_UNICODE};

constexpr SDL_Color ega_colors[]{
	{0x00, 0x00, 0x00, 0xff},			// 0: Black
	{0x00, 0x00, 0xa8, 0xff},			// 1: Blue
	{0x00, 0xa8, 0x00, 0xff},			// 2: Green
	{0x00, 0xa8, 0xa8, 0xff},			// 3: Cyan
	{0xa8, 0x00, 0x00, 0xff},			// 4: Red
	{0xa8, 0x00, 0xa8, 0xff},			// 5: Magenta
	{0xa8, 0x54, 0x00, 0xff},			// 6: Brown
	{0xa8, 0xa8, 0xa8, 0xff},			// 7: Grey
	{0x54, 0x54, 0x54, 0xff},			// 8: Dark grey
	{0x54, 0x54, 0xfe, 0xff},			// 9: Bright blue
	{0x54, 0xfe, 0x54, 0xff},			// 10: Bright green
	{0x54, 0xfe, 0xfe, 0xff},			// 11: Bright cyan
	{0xfe, 0x54, 0x54, 0xff},			// 12: Bright red
	{0xfe, 0x54, 0xfe, 0xff},			// 13: Bright magenta
	{0xfe, 0xfe, 0x54, 0xff},			// 14: Yellow
	{0xfe, 0xfe, 0xfe, 0xff}			// 15: Bright white
};

#ifdef _WIN32
// Examine system DPI settings to determine whether to use the large font.
static int Win32_UseLargeFont()
{
	auto hdc{GetDC(nullptr)};

	if (!hdc)
	{
		return 0;
	}

	auto dpix{GetDeviceCaps(hdc, LOGPIXELSX)};
	ReleaseDC(nullptr, hdc);

	// 144 is the DPI when using "150%" scaling. If the user has this set then consider this an appropriate threshold for using the large font.
	return dpix >= LARGE_FONT_THRESHOLD;
}
#endif // _WIN32

static const FontType* FontForName(std::string name)
{
	const FontType* fonts[]{&small_font, &normal_font, &large_font, &highdpi_font, nullptr};

	for (size_t i{0}; fonts[i]->name; ++i)
	{
		if (!strcmp(fonts[i]->name, name))
		{
			return fonts[i];
		}
	}

	return nullptr;
}

// Select the font to use, based on screen resolution
// If the highest screen resolution available is less than 640x480, use the small font.
static void ChooseFont()
{
	// Allow normal selection to be overridden from an environment variable:
	std::string env{getenv("TEXTSCREEN_FONT")};
	if (env)
	{
		font = FontForName(env);

		if (font)
		{
			return;
		}
	}

	// Get desktop resolution. If in doubt and we can't get a list, always prefer to fall back to the normal font:
	SDL_DisplayMode desktop_info;
	if (SDL_GetCurrentDisplayMode(0, &desktop_info))
	{
		font = &highdpi_font;
		return;
	}

	// On tiny low-res screens (eg. palmtops) use the small font.
	// If the screen resolution is at least 1920x1080, this is a modern high-resolution display, and we can use the large font.
	if (desktop_info.w < 640 || desktop_info.h < 480)
	{
		font = &small_font;
	}

#ifdef _WIN32
	// On Windows we can use the system DPI settings to make a more educated guess about whether to use the large font.
	else if (Win32_UseLargeFont())
	{
		font = &large_font;
	}
#endif // _WIN32

	// TODO: Detect high DPI on Linux by inquiring about Gtk+ scale settings.
	// This looks like it should just be a case of shelling out to invoke the 'gsettings' command,
	// eg. gsettings get org.gnome.desktop.interface text-scaling-factor and using large_font if the result is >= 2.
	else
	{
		// highdpi_font usually means normal_font (the normal resolution version), but actually means
		// "set the HIGHDPI flag and try to use large_font if we initialize successfully".
		font = &highdpi_font;
	}
}

// Initialize text mode screen; Returns 1 if successful, 0 if an error occurred
int Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		return 0;
	}

	ChooseFont();

	screen_image_w = SCREEN_W * font->w;
	screen_image_h = SCREEN_H * font->h;

	int flags{0};
	// If highdpi_font is selected, try to initialize high dpi rendering.
	if (font == &highdpi_font)
	{
		flags |= SDL_WINDOW_ALLOW_HIGHDPI;
	}

	SDLWindow = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_image_w, screen_image_h, flags);

	if (!SDLWindow)
	{
		return 0;
	}

	renderer = SDL_CreateRenderer(SDLWindow, -1, SDL_RENDERER_SOFTWARE);

	// Special handling for OS X retina display. If we successfully set the highdpi flag, check the output size for the screen renderer.
	// If we get the 2x doubled size we expect from a retina display, use the large font for drawing the screen.
	if ((SDL_GetWindowFlags(SDLWindow) & SDL_WINDOW_ALLOW_HIGHDPI) != 0)
	{
		int render_w;
		int render_h;

		if (SDL_GetRendererOutputSize(renderer, &render_w, &render_h) == 0 && render_w >= SCREEN_W * large_font.w && render_h >= SCREEN_H * large_font.h)
		{
			font = &large_font;
			// Note that we deliberately do not update screen_image_{w,h} since these are the dimensions of textscreen image in screen coordinates, not pixels.
		}
	}

	// Failed to initialize for high dpi (retina display) rendering? If so then use the normal resolution font instead.
	if (font == &highdpi_font)
	{
		font = &normal_font;
	}

	// Instead, we draw everything into an intermediate 8-bit surface the same dimensions as the screen.
	// SDL then takes care of all the 8->32 bit (or whatever depth) color conversions for us.
	screenbuffer = SDL_CreateRGBSurface(0, SCREEN_W * font->w, SCREEN_H * font->h, 8, 0, 0, 0, 0);

	SDL_LockSurface(screenbuffer);
	SDL_SetPaletteColors(screenbuffer->format->palette, ega_colors, 0, 16);
	SDL_UnlockSurface(screenbuffer);

	screendata = static_cast<decltype(screendata)>(malloc(SCREEN_W * SCREEN_H * 2));
	memset(screendata, 0, SCREEN_W * SCREEN_H * 2);

	return 1;
}

void Shutdown()
{
	free(screendata);
	screendata = nullptr;
	SDL_FreeSurface(screenbuffer);
	screenbuffer = nullptr;
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void SetColor(ColorType color, int r, int g, int b)
{
	SDL_Color c{r, g, b, 0xff};

	SDL_LockSurface(screenbuffer);
	SDL_SetPaletteColors(screenbuffer->format->palette, &c, (int)color, 1);
	SDL_UnlockSurface(screenbuffer);
}

unsigned char* GetScreenData()
{
	return screendata;
}

static inline void UpdateCharacter(int x, int y)
{
	const uint8_t* p{&screendata[(y * SCREEN_W + x) * 2]};
	unsigned char character{p[0]};

	int fg{p[1] & 0xf};
	int bg{(p[1] >> 4) & 0xf};

	if (bg & 0x8)
	{
		// blinking
		bg &= ~0x8;

		if (((SDL_GetTicks() / BLINK_PERIOD) % 2) == 0)
		{
			fg = bg;
		}
	}

	// How many bytes per line?
	p = &font->data[(character * font->w * font->h) / 8];
	unsigned bit{0};

	unsigned char* s = ((unsigned char*)screenbuffer->pixels) + (y * font->h * screenbuffer->pitch) + (x * font->w);

	for (size_t y1{0}; y1 < font->h; ++y1)
	{
		unsigned char* s1 = s;

		for (size_t x1{0}; x1 < font->w; ++x1)
		{
			if (*p & (1 << bit))
			{
				*s1 = fg;
				++s1;
			}
			else
			{
				*s1 = bg;
				++s1;
			}

			++bit;
			if (bit == 8)
			{
				++p;
				bit = 0;
			}
		}

		s += screenbuffer->pitch;
	}
}

static int LimitToRange(int val, int min, int max)
{
	if (val < min)
	{
		return min;
	}
	else if (val > max)
	{
		return max;
	}
	else
	{
		return val;
	}
}

static void GetDestRect(SDL_Rect* rect)
{
	int w;
	int h;

	SDL_GetRendererOutputSize(renderer, &w, &h);
	rect->x = (w - screenbuffer->w) / 2;
	rect->y = (h - screenbuffer->h) / 2;
	rect->w = screenbuffer->w;
	rect->h = screenbuffer->h;
}

void UpdateScreenArea(int x, int y, int w, int h)
{
	SDL_LockSurface(screenbuffer);

	auto x_end{LimitToRange(x + w, 0, SCREEN_W)};
	auto y_end{LimitToRange(y + h, 0, SCREEN_H)};
	x = LimitToRange(x, 0, SCREEN_W);
	y = LimitToRange(y, 0, SCREEN_H);

	for (size_t y1{y}; y1 < y_end; ++y1)
	{
		for (size_t x1{x}; x1 < x_end; ++x1)
		{
			UpdateCharacter(x1, y1);
		}
	}

	SDL_UnlockSurface(screenbuffer);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	// TODO: This is currently creating a new texture every time we render the screen; find a more efficient way to do it.
	auto screentx{SDL_CreateTextureFromSurface(renderer, screenbuffer)};

	SDL_RenderClear(renderer);

	SDL_Rect rect;
	GetDestRect(&rect);

	SDL_RenderCopy(renderer, screentx, nullptr, &rect);
	SDL_RenderPresent(renderer);

	SDL_DestroyTexture(screentx);
}

void UpdateScreen()
{
	UpdateScreenArea(0, 0, SCREEN_W, SCREEN_H);
}

void GetMousePosition(int* x, int* y)
{
	SDL_GetMouseState(x, y);

	// Translate mouse position from 'pixel' position into character position. We are working here in screen coordinates and not pixels,
	// since this is what SDL_GetWindowSize() returns; we must calculate and subtract the origin position since we center the image within the window.
	int window_w;
	int window_h;
	SDL_GetWindowSize(SDLWindow, &window_w, &window_h);
	int origin_x{(window_w - screen_image_w) / 2};
	int origin_y{(window_h - screen_image_h) / 2};
	*x = ((*x - origin_x) * SCREEN_W) / screen_image_w;
	*y = ((*y - origin_y) * SCREEN_H) / screen_image_h;

	if (*x < 0)
	{
		*x = 0;
	}
	else if (*x >= SCREEN_W)
	{
		*x = SCREEN_W - 1;
	}
	if (*y < 0)
	{
		*y = 0;
	}
	else if (*y >= SCREEN_H)
	{
		*y = SCREEN_H - 1;
	}
}

// Translates the SDL key

// XXX: duplicate from doomtype.h
#define arrlen(array) (sizeof(array) / sizeof(*array))

static int TranslateScancode(SDL_Scancode scancode)
{
	switch (scancode)
	{
	case SDL_SCANCODE_LCTRL:
	case SDL_SCANCODE_RCTRL:
		return KEY_RCTRL;

	case SDL_SCANCODE_LSHIFT:
	case SDL_SCANCODE_RSHIFT:
		return KEY_RSHIFT;

	case SDL_SCANCODE_LALT:
		return KEY_LALT;

	case SDL_SCANCODE_RALT:
		return KEY_RALT;

	default:
		if (scancode < arrlen(scancode_translate_table))
		{
			return scancode_translate_table[scancode];
		}
		else
		{
			return 0;
		}
	}
}

static int TranslateKeysym(const SDL_Keysym* sym)
{
	// We cheat here and make use of TranslateScancode. The range of keys associated with printable characters is pretty contiguous,
	// so if it's inside that range we want the localized version of the key instead.
	auto translated{TranslateScancode(sym->scancode)};

	if (translated >= 0x20 && translated < 0x7f)
	{
		return sym->sym;
	}
	else
	{
		return translated;
	}
}

// Convert an SDL button index to textscreen button index.
// Note special cases because 2 == mid in SDL, 3 == mid in textscreen/setup
static int SDLButtonToTXTButton(int button)
{
	switch (button)
	{
	case SDL_BUTTON_LEFT:
		return MOUSE_LEFT;
	case SDL_BUTTON_RIGHT:
		return MOUSE_RIGHT;
	case SDL_BUTTON_MIDDLE:
		return MOUSE_MIDDLE;
	default:
		return MOUSE_BASE + button - 1;
	}
}

// Convert an SDL wheel motion to a textscreen button index.
static int SDLWheelToTXTButton(const SDL_MouseWheelEvent* wheel)
{
	if (wheel->y <= 0)
	{
		return MOUSE_SCROLLDOWN;
	}
	else
	{
		return MOUSE_SCROLLUP;
	}
}

static bool MouseHasMoved()
{
	static int last_x{0};
	static int last_y{0};

	int x;
	int y;
	GetMousePosition(&x, &y);

	if (x != last_x || y != last_y)
	{
		last_x = x;
		last_y = y;
		return true;
	}
	else
	{
		return false;
	}
}

int GetChar()
{
	SDL_Event ev;
	while (SDL_PollEvent(&ev))
	{
		// If there is an event callback, allow it to intercept this event.
		if (event_callback)
		{
			if (event_callback(&ev, event_callback_data))
			{
				continue;
			}
		}

		// Process the event.
		switch (ev.type)
		{
		case SDL_EventType::SDL_MOUSEBUTTONDOWN:
			if (ev.button.button < MAX_MOUSE_BUTTONS)
			{
				return SDLButtonToTXTButton(ev.button.button);
			}
			break;

		case SDL_EventType::SDL_MOUSEWHEEL:
			return SDLWheelToTXTButton(&ev.wheel);

		case SDL_EventType::SDL_KEYDOWN:
			switch (input_mode)
			{
			case InputType::raw:
				return TranslateScancode(ev.key.keysym.scancode);
			case InputType::normal:
				return TranslateKeysym(&ev.key.keysym);
			case InputType::text:
				// We ignore key inputs in this mode, except for a few special cases needed during text input:
				if (ev.key.keysym.sym == SDL_KeyCode::SDLK_ESCAPE
					|| ev.key.keysym.sym == SDL_KeyCode::SDLK_BACKSPACE
					|| ev.key.keysym.sym == SDL_KeyCode::SDLK_RETURN)
				{
					return TranslateKeysym(&ev.key.keysym);
				}
				break;
			}
			break;

		case SDL_EventType::SDL_TEXTINPUT:
			if (input_mode == InputType::text)
			{
				// TODO: Support input of more than just the first char?
				std::string p{ev.text.text};
				int result{DecodeUTF8(&p)};
				// 0-127 is ASCII, but we map non-ASCII Unicode chars into a higher range to avoid conflicts with special keys.
				return UNICODE_TO_KEY(result);
			}
			break;

		case SDL_EventType::SDL_QUIT:
			// Quit = escape
			return 27;

		case SDL_EventType::SDL_MOUSEMOTION:
			if (MouseHasMoved())
			{
				return 0;
			}

		default:
			break;
		}
	}

	return -1;
}

int GetModifierState(ModifierType mod)
{
	SDL_Keymod state;

	state = SDL_GetModState();

	switch (mod)
	{
	case ModifierType::shift:
		return (state & KMOD_SHIFT) != 0;
	case ModifierType::ctrl:
		return (state & KMOD_CTRL) != 0;
	case ModifierType::alt:
		return (state & KMOD_ALT) != 0;
	default:
		return 0;
	}
}

int UnicodeCharacter(unsigned c)
{
	// Check the code page mapping to see if this character maps to anything.
	for (size_t i{0}; i < arrlen(code_page_to_unicode); ++i)
	{
		if (code_page_to_unicode[i] == c)
		{
			return i;
		}
	}

	return -1;
}

// Returns true if the given UTF8 key name is printable to the screen.
static int PrintableName(std::string s)
{
	std::string p = s;
	while (*p != '\0')
	{
		unsigned c{DecodeUTF8(&p)};
		if (UnicodeCharacter(c) < 0)
		{
			return 0;
		}
	}

	return 1;
}

static std::string NameForKey(int key)
{
	// Overrides purely for aesthetical reasons, so that default window accelerator keys match those of setup.exe.
	switch (key)
	{
	case KEY_ESCAPE: return "ESC";
	case KEY_ENTER: return "ENTER";
	default:
		break;
	}

	// This key presumably maps to a scan code that is listed in the
	// translation table. Find which mapping and once we have a scancode,
	// we can convert it into a virtual key, then a string via SDL.
	for (size_t i{0}; i < arrlen(scancode_translate_table); ++i)
	{
		if (scancode_translate_table[i] == key)
		{
			std::string result = SDL_GetKeyName(SDL_GetKeyFromScancode(i));
			if (UTF8_Strlen(result) > 6 || !PrintableName(result))
			{
				break;
			}
			return result;
		}
	}

	// Use US English fallback names, if the localized name is too long, not found in the scancode table, or contains unprintable chars
	// (non-extended ASCII character set):
	for (size_t i{0}; i < arrlen(key_names); ++i)
	{
		if (key_names[i].key == key)
		{
			return key_names[i].name;
		}
	}

	return nullptr;
}

void GetKeyDescription(int key, std::string buf, size_t buf_len)
{
	std::string keyname{NameForKey(key)};

	if (keyname)
	{
		StringCopy(buf, keyname, buf_len);

		// Key description should be all-uppercase to match setup.exe.
		for (size_t i{0}; buf[i] != '\0'; ++i)
		{
			buf[i] = toupper(buf[i]);
		}
	}
	else
	{
		snprintf(buf, buf_len, "??%i", key);
	}
}

// Searches the desktop screen buffer to determine whether there are any blinking characters.
bool ScreenHasBlinkingChars()
{
	// Check all characters in screen buffer
	for (size_t y{0}; y < SCREEN_H; ++y)
	{
		for (size_t x{0}; x < SCREEN_W; ++x)
		{
			unsigned char* p{&screendata[(y * SCREEN_W + x) * 2]};

			if (p[1] & 0x80)
			{
				// This character is blinking
				return true;
			}
		}
	}

	// None found
	return false;
}

// Sleeps until an event is received, the screen needs to be redrawn, or until timeout expires (if timeout != 0)
void Sleep(TimeType timeout)
{
	if (ScreenHasBlinkingChars())
	{
		TimeType time_to_next_blink{BLINK_PERIOD - (SDL_GetTicks() % BLINK_PERIOD)};

		// There are blinking characters on the screen, so we must time out after a while
		if (timeout == 0 || timeout > time_to_next_blink)
		{
			// Add one so it is always positive
			timeout = time_to_next_blink + 1;
		}
	}

	if (timeout == 0)
	{
		// We can just wait forever until an event occurs
		SDL_WaitEvent(nullptr);
	}
	else
	{
		// Sit in a busy loop until the timeout expires or we have to redraw the blinking screen
		TimeType start_time{SDL_GetTicks()};

		while (SDL_GetTicks() < start_time + timeout)
		{
			if (SDL_PollEvent(nullptr) != 0)
			{
				// Received an event, so stop waiting
				break;
			}

			// Don't hog the CPU
			SDL_Delay(1);
		}
	}
}

void SetInputMode(InputType mode)
{
	if (mode == InputType::text && !SDL_IsTextInputActive())
	{
		SDL_StartTextInput();
	}
	else if (SDL_IsTextInputActive() && mode != InputType::text)
	{
		SDL_StopTextInput();
	}

	input_mode = mode;
}

void SetWindowTitle(std::string title)
{
	SDL_SetWindowTitle(SDLWindow, title.c_str());
}

void SDL_SetEventCallback(TxtSDLEventCallbackFunc callback, void* user_data)
{
	event_callback = callback;
	event_callback_data = user_data;
}

// Safe string functions.
void StringCopy(std::string dest, std::string src, size_t dest_len)
{
	if (dest_len < 1)
	{
		return;
	}

	dest[dest_len - 1] = '\0';
	strncpy(dest, src, dest_len - 1);
}

void StringConcat(std::string dest, std::string src, size_t dest_len)
{
	size_t offset{strlen(dest)};
	if (offset > dest_len)
	{
		offset = dest_len;
	}

	StringCopy(dest + offset, src, dest_len - offset);
}

// Safe, portable vsnprintf().
int vsnprintf(std::string buf, size_t buf_len, std::string s, va_list args)
{
	if (buf_len < 1)
	{
		return 0;
	}

	// Windows (and other OSes?) has a vsnprintf() that doesn't always append a trailing \0.
	// So we must do it, and write into a buffer that is one byte shorter; otherwise this function is unsafe.
	auto result{vsnprintf(buf, buf_len, s, args)};

	// If truncated, change the final char in the buffer to a \0. A negative result indicates a truncated buffer on Windows.
	if (result < 0 || result >= buf_len)
	{
		buf[buf_len - 1] = '\0';
		result = buf_len - 1;
	}

	return result;
}

// Safe, portable snprintf().
int snprintf(std::string buf, size_t buf_len, std::string s, ...)
{
	va_list args;
	int result;
	va_start(args, s);
	result = vsnprintf(buf, buf_len, s, args);
	va_end(args);
	return result;
}

} // END NAMESPACE cudadoom::txt
