/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Routines for selecting files.
\**********************************************************************************************************************************************/

#include "doomkeys.h"

#include "txt_main.h"
#include "txt_widget.h"
#include "txt_inputbox.h"
#include "txt_io.h"
#include "txt_gui.h"

#include "txt_fileselect.h"

namespace cudadoom::txt
{

// Dummy value to select a directory.
const char* DIRECTORY[] = { "__directory__", NULL };

#ifndef _WIN32

static char* ExecReadOutput(char** argv)
{
	char* result;
	int completed;
	int pid, status, result_len;
	int pipefd[2];

	if (pipe(pipefd) != 0)
	{
		return NULL;
	}

	pid = fork();

	if (pid == 0)
	{
		dup2(pipefd[1], fileno(stdout));
		execv(argv[0], argv);
		exit(-1);
	}

	fcntl(pipefd[0], F_SETFL, O_NONBLOCK);

	// Read program output into 'result' string.
	// Wait until the program has completed and (if it was successful) a full line has been read.
	result = NULL;
	result_len = 0;
	completed = 0;

	while (!completed
		|| (status == 0 && (result == NULL || strchr(result, '\n') == NULL)))
	{
		char buf[64];
		int bytes;

		if (!completed && waitpid(pid, &status, WNOHANG) != 0)
		{
			completed = 1;
		}

		bytes = read(pipefd[0], buf, sizeof(buf));

		if (bytes < 0)
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
			{
				status = -1;
				break;
			}
		}
		else
		{
			char* new_result = realloc(result, result_len + bytes + 1);
			if (new_result == NULL)
			{
				break;
			}
			result = new_result;
			memcpy(result + result_len, buf, bytes);
			result_len += bytes;
			result[result_len] = '\0';
		}

		usleep(100 * 1000);
		Sleep(1);
		UpdateScreen();
	}

	close(pipefd[0]);
	close(pipefd[1]);

	// Must have a success exit code.
	if (WEXITSTATUS(status) != 0)
	{
		free(result);
		result = NULL;
	}

	// Strip off newline from the end.
	if (result != NULL && result[result_len - 1] == '\n')
	{
		result[result_len - 1] = '\0';
	}

	return result;
}

#endif

// This is currently disabled on Windows because it doesn't work.
// Current issues:
//	* On Windows Vista+ the mouse cursor freezes when the dialog is
//		opened. This is probably some conflict with SDL (might be
//		resolved by opening the dialog in a separate thread so that
//		UpdateScreen can be run in the background).
//	* On Windows XP the program exits/crashes when the dialog is
//		closed.
#if defined(_WIN32)

int CanSelectFiles()
{
	return 0;
}

char* SelectFile(const char* window_title, const char** extensions)
{
	return NULL;
}

#elif defined(xxxdisabled_WIN32)

// Windows code. Use comdlg32 to pop up a dialog box.
static bool (*MyGetOpenFileName)(LPOPENFILENAME) = NULL;
static LPITEMIDLIST (*MySHBrowseForFolder)(LPBROWSEINFO) = NULL;
static bool (*MySHGetPathFromIDList)(LPITEMIDLIST, LPTSTR) = NULL;

// Load library functions from DLL files.
static int LoadDLLs()
{
	HMODULE comdlg32, shell32

	comdlg32 = LoadLibraryW(L"comdlg32.dll");
	if (comdlg32 == NULL)
	{
		return 0;
	}

	shell32 = LoadLibraryW(L"shell32.dll");
	if (shell32 == NULL)
	{
		FreeLibrary(comdlg32);
		return 0;
	}

	MyGetOpenFileName =
		(void*) GetProcAddress(comdlg32, "GetOpenFileNameA");
	MySHBrowseForFolder =
		(void*) GetProcAddress(shell32, "SHBrowseForFolder");
	MySHGetPathFromIDList =
		(void*) GetProcAddress(shell32, "SHGetPathFromIDList");

	return MyGetOpenFileName != NULL
		&& MySHBrowseForFolder != NULL
		&& MySHGetPathFromIDList != NULL;
}

static int InitLibraries()
{
	static int initted = 0, success = 0;

	if (!initted)
	{
		success = LoadDLLs();
		initted = 1;
	}

	return success;
}

// Generate the "filter" string from the list of extensions.
static char* GenerateFilterString(const char** extensions)
{
	unsigned result_len = 1;
	unsigned i;
	char* result, *out;
	size_t out_len, offset;

	if (extensions == NULL)
	{
		return NULL;
	}

	for (i = 0; extensions[i] != NULL; ++i)
	{
		result_len += 16 + strlen(extensions[i]) * 3;
	}

	result = static_cast<decltype(result)>(malloc(result_len));
	out = result; out_len = result_len;

	for (i = 0; extensions[i] != NULL; ++i)
	{
		// .wad files (*.wad)\0
		offset = snprintf(out, out_len, "%s files (*.%s)",
								extensions[i], extensions[i]);
		out += offset + 1; out_len -= offset + 1;

		// *.wad\0
		offset = snprintf(out, out_len, "*.%s", extensions[i]);
		out_len += offset + 1; out_len -= offset + 1;
	}

	*out = '\0';

	return result;
}

int CanSelectFiles()
{
	return InitLibraries();
}

static char* SelectDirectory(char* window_title)
{
	LPITEMIDLIST pidl;
	BROWSEINFO bi;
	char selected[MAX_PATH] = "";
	char* result;

	ZeroMemory(&bi, sizeof(bi));
	bi.hwndOwner = NULL;
	bi.lpszTitle = window_title;
	bi.pszDisplayName = selected;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

	pidl = MySHBrowseForFolder(&bi);

	result = NULL;

	if (pidl != NULL)
	{
		if (MySHGetPathFromIDList(pidl, selected))
		{
			result = strdup(selected);
		}

		// TODO: Free pidl
	}

	return result;
}

char* SelectFile(const char* window_title, const char** extensions)
{
	OPENFILENAME fm;
	char selected[MAX_PATH] = "";
	char* filter_string, *result;

	if (!InitLibraries())
	{
		return NULL;
	}

	if (extensions == DIRECTORY)
	{
		return SelectDirectory(window_title);
	}

	filter_string = GenerateFilterString(extensions);

	ZeroMemory(&fm, sizeof(fm));
	fm.lStructSize = sizeof(OPENFILENAME);
	fm.hwndOwner = NULL;
	fm.lpstrTitle = window_title;
	fm.lpstrFilter = filter_string;
	fm.lpstrFile = selected;
	fm.nMaxFile = MAX_PATH;
	fm.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	fm.lpstrDefExt = "";

	if (!MyGetOpenFileName(&fm))
	{
		result = NULL;
	}
	else
	{
		result = strdup(selected);
	}

	free(filter_string);

	return result;
}

#elif defined(__MACOSX__)

// Mac OS X code. Popping up a dialog requires Objective C/Cocoa
// but we can get away with using AppleScript which avoids adding
// an Objective C dependency. This is rather silly.

// Printf format string for the "wrapper" portion of the AppleScript:
#define APPLESCRIPT_WRAPPER "copy POSIX path of (%s) to stdout"

static char* CreateEscapedString(const char* original)
{
	char* result;
	const char* in;
	char* out;

	// We need to take care not to overflow the buffer, so count exactly.
#define ESCAPED_CHARS "\"\\"
	size_t count_extras = 2;	// start counting the two quotes
	for (in = original; *in; ++in)
	{
		if (strchr(ESCAPED_CHARS, *in))
		{
			++count_extras;
		}
	}

	result = static_cast<decltype(result)>(malloc(strlen(original) + count_extras + 1));
	if (!result)
	{
		return NULL;
	}
	out = result;
	*(out++) = '"';
	for (in = original; *in; ++in)
	{
		if (strchr(ESCAPED_CHARS, *in))
		{
			*(out++) = '\\';
		}
		*(out++) = *in;
	}
	*(out++) = '"';
	*out = 0;

	return result;
#undef ESCAPED_CHARS
}

// Build list of extensions, like: {"wad","lmp","txt"}
static char* CreateExtensionsList(const char** extensions)
{
	char* result, *escaped;
	unsigned result_len;
	unsigned i;

	if (extensions == NULL)
	{
		return NULL;
	}

	result_len = 3;
	for (i = 0; extensions[i] != NULL; ++i)
	{
		result_len += 5 + strlen(extensions[i]) * 2;
	}

	result = static_cast<decltype(result)>(malloc(result_len));
	if (!result)
	{
		return NULL;
	}
	StringCopy(result, "{", result_len);

	for (i = 0; extensions[i] != NULL; ++i)
	{
		escaped = CreateEscapedString(extensions[i]);
		if (!escaped)
		{
			free(result);
			return NULL;
		}
		StringConcat(result, escaped, result_len);
		free(escaped);

		if (extensions[i + 1] != NULL)
		{
			StringConcat(result, ",", result_len);
		}
	}

	StringConcat(result, "}", result_len);

	return result;
}

static char* GenerateSelector(const char* const window_title, const char** extensions)
{
	const char* chooser;
	char* ext_list = NULL;
	char* window_title_escaped = NULL;
	char* result = NULL;
	unsigned result_len = 64;

	if (extensions == DIRECTORY)
	{
		chooser = "choose folder";
	}
	else
	{
		chooser = "choose file";
		ext_list = CreateExtensionsList(extensions);
		if (!ext_list)
		{
			return NULL;
		}
	}

	// Calculate size.
	if (window_title != NULL)
	{
		window_title_escaped = CreateEscapedString(window_title);
		if (!window_title_escaped)
		{
			free(ext_list);
			return NULL;
		}
		result_len += strlen(window_title_escaped);
	}
	if (ext_list != NULL)
	{
		result_len += strlen(ext_list);
	}

	result = static_cast<decltype(result)>(malloc(result_len));
	if (!result)
	{
		free(window_title_escaped);
		free(ext_list);
		return NULL;
	}

	StringCopy(result, chooser, result_len);

	if (window_title_escaped != NULL)
	{
		StringConcat(result, " with prompt ", result_len);
		StringConcat(result, window_title_escaped, result_len);
	}

	if (ext_list != NULL)
	{
		StringConcat(result, " of type ", result_len);
		StringConcat(result, ext_list, result_len);
	}

	free(window_title_escaped);
	free(ext_list);
	return result;
}

static char* GenerateAppleScript(const char* window_title, const char** extensions)
{
	char* selector, *result;
	size_t result_len;

	selector = GenerateSelector(window_title, extensions);
	if (!selector)
	{
		return NULL;
	}

	result_len = strlen(APPLESCRIPT_WRAPPER) + strlen(selector);
	result = static_cast<decltype(result)>(malloc(result_len));
	if (!result)
	{
		free(selector);
		return NULL;
	}

	snprintf(result, result_len, APPLESCRIPT_WRAPPER, selector);
	free(selector);

	return result;
}

int CanSelectFiles()
{
	return 1;
}

char* SelectFile(const char* window_title, const char** extensions)
{
	char* argv[4];
	char* result, *applescript;

	applescript = GenerateAppleScript(window_title, extensions);
	if (!applescript)
	{
		return NULL;
	}

	argv[0] = "/usr/bin/osascript";
	argv[1] = "-e";
	argv[2] = applescript;
	argv[3] = NULL;

	result = ExecReadOutput(argv);

	free(applescript);

	return result;
}

#else

// Linux version: invoke the Zenity command line program to pop up a dialog box. This avoids adding Gtk+ as a compile dependency.
#define ZENITY_BINARY "/usr/bin/zenity"

static unsigned NumExtensions(const char** extensions)
{
	unsigned result = 0;

	if (extensions != NULL)
	{
		for (result = 0; extensions[result] != NULL; ++result);
	}

	return result;
}

static int ZenityAvailable()
{
	return system(ZENITY_BINARY " --help >/dev/null 2>&1") == 0;
}

int CanSelectFiles()
{
	return ZenityAvailable();
}

// ExpandExtension
// given an extension (like wad)
// return a pointer to a string that is a case-insensitive
// pattern representation (like [Ww][Aa][Dd])
static char* ExpandExtension(const char* orig)
{
	int oldlen, newlen, i;
	char* c,* newext = NULL;

	oldlen = strlen(orig);
	newlen = oldlen * 4; // pathological case: 'w' => '[Ww]'
	newext = static_cast<decltype(newext)>(malloc(newlen+1));

	if (newext == NULL)
	{
		return NULL;
	}

	c = newext;
	for (i = 0; i < oldlen; ++i)
	{
		if (isalpha(orig[i]))
		{
			*(c++) = '[';
			*(c++) = tolower(orig[i]);
			*(c++) = toupper(orig[i]);
			*(c++) = ']';
		}
		else
		{
			*(c++) = orig[i];
		}
	}
	*c = '\0';
	return newext;
}

char* SelectFile(const char* window_title, const char** extensions)
{
	unsigned i;
	size_t len;
	char* result;
	char** argv;
	int argc;

	if (!ZenityAvailable())
	{
		return NULL;
	}

	argv = calloc(5 + NumExtensions(extensions), sizeof(char*));
	argv[0] = strdup(ZENITY_BINARY);
	argv[1] = strdup("--file-selection");
	argc = 2;

	if (window_title != NULL)
	{
		len = 10 + strlen(window_title);
		argv[argc] = static_cast<decltype(argv[argc])>(malloc(len));
		snprintf(argv[argc], len, "--title=%s", window_title);
		++argc;
	}

	if (extensions == DIRECTORY)
	{
		argv[argc] = strdup("--directory");
		++argc;
	}
	else if (extensions != NULL)
	{
		for (i = 0; extensions[i] != NULL; ++i)
		{
			char* newext = ExpandExtension(extensions[i]);
			if (newext)
			{
				len = 30 + strlen(extensions[i]) + strlen(newext);
				argv[argc] = static_cast<decltype(argv[argc])>(malloc(len));
				snprintf(argv[argc], len, "--file-filter=.%s | *.%s",
								extensions[i], newext);
				++argc;
				free(newext);
			}
		}

		argv[argc] = strdup("--file-filter=*.* | *.*");
		++argc;
	}

	argv[argc] = NULL;

	result = ExecReadOutput(argv);

	for (i = 0; i < argc; ++i)
	{
		free(argv[i]);
	}

	free(argv);

	return result;
}

#endif

static void FileSelectSizeCalc(UNCAST_ARG(fileselect))
{
	CAST_ARG(txt_fileselect_t, fileselect);

	// Calculate widget size, but override the width to always
	// be the configured size.

	CalcWidgetSize(fileselect->inputbox);
	fileselect->widget.w = fileselect->size;
	fileselect->widget.h = fileselect->inputbox->widget.h;
}

static void FileSelectDrawer(UNCAST_ARG(fileselect))
{
	CAST_ARG(txt_fileselect_t, fileselect);

	// Input box widget inherits all the properties of the
	// file selector.

	fileselect->inputbox->widget.x = fileselect->widget.x + 2;
	fileselect->inputbox->widget.y = fileselect->widget.y;
	fileselect->inputbox->widget.w = fileselect->widget.w - 2;
	fileselect->inputbox->widget.h = fileselect->widget.h;

	// Triple bar symbol gives a distinguishing look to the file selector.
	DrawCodePageString("\xf0 ");
	BGColor(ColorType::black, false);
	DrawWidget(fileselect->inputbox);
}

static void FileSelectDestructor(UNCAST_ARG(fileselect))
{
	CAST_ARG(txt_fileselect_t, fileselect);

	DestroyWidget(fileselect->inputbox);
}

static int DoSelectFile(txt_fileselect_t* fileselect)
{
	char* path;
	char** var;

	if (CanSelectFiles())
	{
		path = SelectFile(fileselect->prompt,
								fileselect->extensions);

		// Update inputbox variable. If cancel was pressed (ie. NULL was returned by SelectFile) then reset to empty string, not NULL).
		if (path == NULL)
		{
			path = strdup("");
		}

		var = fileselect->inputbox->value;
		free(*var);
		*var = path;
		return 1;
	}

	return 0;
}

static int FileSelectKeyPress(UNCAST_ARG(fileselect), int key)
{
	CAST_ARG(txt_fileselect_t, fileselect);

	// When the enter key is pressed, pop up a file selection dialog,
	// if file selectors work. Allow holding down 'alt' to override
	// use of the native file selector, so the user can just type a path.
	if (!fileselect->inputbox->editing && !GetModifierState(ModifierType::alt) && key == KEY_ENTER)
	{
		if (DoSelectFile(fileselect))
		{
			return 1;
		}
	}

	return WidgetKeyPress(fileselect->inputbox, key);
}

static void FileSelectMousePress(UNCAST_ARG(fileselect), int x, int y, int b)
{
	CAST_ARG(txt_fileselect_t, fileselect);

	if (!fileselect->inputbox->editing && !GetModifierState(ModifierType::alt) && b == MOUSE_LEFT)
	{
		if (DoSelectFile(fileselect))
		{
			return;
		}
	}

	WidgetMousePress(fileselect->inputbox, x, y, b);
}

static void FileSelectFocused(UNCAST_ARG(fileselect), int focused)
{
	CAST_ARG(txt_fileselect_t, fileselect);

	SetWidgetFocus(fileselect->inputbox, focused);
}

WidgetClass txt_fileselect_class =
{
	AlwaysSelectable,
	FileSelectSizeCalc,
	FileSelectDrawer,
	FileSelectKeyPress,
	FileSelectDestructor,
	FileSelectMousePress,
	NULL,
	FileSelectFocused,
};

// If the (inner) inputbox widget is changed, emit a change to the outer (fileselect) widget.
static void InputBoxChanged(UNCAST_ARG(widget), UNCAST_ARG(fileselect))
{
	CAST_ARG(txt_fileselect_t, fileselect);

	EmitSignal(&fileselect->widget, "changed");
}

txt_fileselect_t* NewFileSelector(char** variable, int size, const char* prompt, const char** extensions)
{
	txt_fileselect_t* fileselect;

	fileselect = static_cast<decltype(fileselect)>(malloc(sizeof(txt_fileselect_t)));
	InitWidget(fileselect, &txt_fileselect_class);
	fileselect->inputbox = NewInputBox(variable, 1024);
	fileselect->inputbox->widget.parent = &fileselect->widget;
	fileselect->size = size;
	fileselect->prompt = prompt;
	fileselect->extensions = extensions;

	SignalConnect(fileselect->inputbox, "changed", InputBoxChanged, fileselect);

	return fileselect;
}

} /* END NAMESPACE cudadoom::txt */
