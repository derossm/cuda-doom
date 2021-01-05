/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Routines for selecting files, and the txt_fileselect_t widget.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"
#include "../derma/d_native.h"
#include "txt_common.h"

#include "doomkeys.h"

#include "txt_widget.h"
#include "txt_inputbox.h"

#include "txt_main.h"
#include "txt_io.h"
#include "txt_gui.h"

namespace cudadoom::txt
{

/**
 * File selection widget.
 *
 * A file selection widget resembles an input box (@ref txt_inputbox_t)
 * but opens a file selector dialog box when clicked.
 */

 //class FileSelect : public Widget<FileSelect>
class FileSelect : public InputBox<FileSelect>
{
	Widget widget;
	txt_inputbox_t* inputbox;
	int size;
	std::string prompt;
	CHAR_PTR* extensions;

	// Dummy value to select a directory.
	std::string DIRECTORY[] = {"__directory__"};

public:

	FileSelect() : widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy}
	{}

	FileSelect(char** _variable, int _size, std::string _prompt, CHAR_PTR* _extensions) :
		widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy},
	{
		//fileselect = static_cast<decltype(fileselect)>(malloc(sizeof(txt_fileselect_t)));
		//InitWidget(fileselect, &txt_fileselect_class);
		//inputbox = NewInputBox(variable, 1024);
		//inputbox->widget.parent = &widget;
		size = _size;
		prompt = _prompt;
		extensions = _extensions;

		//SignalConnect("changed", InputBoxChanged);
	}

		bool Selectable() noexcept override final
	{
		return true;
	}

	void CalculateSize() noexcept override final
	{
		// Calculate widget size, but override the width to always
		// be the configured size.

		//CalcWidgetSize();
		//widget.w = size;
		//widget.h = inputbox->widget.h;
	}

	void Draw() noexcept override final
	{
		// Input box widget inherits all the properties of the
		// file selector.

		//inputbox->widget.x = widget.x + 2;
		//inputbox->widget.y = widget.y;
		//inputbox->widget.w = widget.w - 2;
		//inputbox->widget.h = widget.h;

		// Triple bar symbol gives a distinguishing look to the file selector.
		DrawCodePageString("\xf0 ");
		BGColor(ColorType::black, false);
		//DrawWidget(inputbox);
	}

	bool KeyPress(Keytype key) noexcept override final
	{
		if (key == KEY_ENTER || key == ' ')
		{
			EmitSignal("changed");
			return true;
		}

		return false;
		// When the enter key is pressed, pop up a file selection dialog,
		// if file selectors work. Allow holding down 'alt' to override
		// use of the native file selector, so the user can just type a path.
		//if (!editing && !GetModifierState(ModifierType::alt) && key == KEY_ENTER)
		//{
			//if (DoSelectFile())
			//{
				//return true;
			//}
		//}

		//return false;
	}

	bool MousePress(MouseEvent evt) noexcept override final
	{
		if (evt.button == MOUSE_LEFT)
		{
			// Equivalent to pressing enter
			return KeyPress(KEY_ENTER);
		}

		return false
			//if (!editing && !GetModifierState(ModifierType::alt) && evt.button == MOUSE_LEFT)
			//{
				//if (DoSelectFile())
				//{
					//return true;
				//}
			//}

			//WidgetMousePress(inputbox, x, y, b);
			//return false;
	}

	void SetLayout() noexcept override final
	{}

	void SetFocus(bool _focus) noexcept override final
	{}

	void Destroy() noexcept override final
	{}

	bool DoSelectFile()
	{
		std::string path;
		char** var;

		if (CanSelectFiles())
		{
			path = SelectFile(prompt, extensions);

			// Update inputbox variable. If cancel was pressed (ie. NULL was returned by SelectFile) then reset to empty string, not NULL).
			if (path.empty)
			{
				path = strdup("");
			}

			var = value;
			free(*var);
			*var = path;
			return 1;
		}

		return 0;
	}

	void FileSelectFocused(int focused)
	{
		//SetWidgetFocus(inputbox, focused);
	}

	// If the (inner) inputbox widget is changed, emit a change to the outer (fileselect) widget.
	void InputBoxChanged()
	{
		//EmitSignal("changed");
	}

	// This is currently disabled on Windows because it doesn't work.
	// Current issues:
	//	* On Windows Vista+ the mouse cursor freezes when the dialog is
	//		opened. This is probably some conflict with SDL (might be
	//		resolved by opening the dialog in a separate thread so that
	//		UpdateScreen can be run in the background).
	//	* On Windows XP the program exits/crashes when the dialog is
	//		closed.

#if defined(_WIN32)
	bool CanSelectFiles()
	{
		return false;
	}

	std::string SelectFile(std::string window_title, CHAR_PTR* extensions)
	{
		return std::string{};
	}

#elif defined(xxxdisabled_WIN32)
	// Windows code. Use comdlg32 to pop up a dialog box.
	static bool (*MyGetOpenFileName)(LPOPENFILENAME) = NULL;
	static LPITEMIDLIST(*MySHBrowseForFolder)(LPBROWSEINFO) = NULL;
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
			(void*)GetProcAddress(comdlg32, "GetOpenFileNameA");
		MySHBrowseForFolder =
			(void*)GetProcAddress(shell32, "SHBrowseForFolder");
		MySHGetPathFromIDList =
			(void*)GetProcAddress(shell32, "SHGetPathFromIDList");

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
	std::string GenerateFilterString(CHAR_PTR* extensions)
	{
		unsigned result_len = 1;
		unsigned i;
		char* result, * out;
		size_t out_len, offset;

		if (extensions == NULL)
		{
			return nullptr;
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

	std::string SelectDirectory(std::string window_title)
	{
		LPITEMIDLIST pidl;
		BROWSEINFO bi;
		char selected[MAX_PATH] = "";
		std::string result;

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

	std::string SelectFile(std::string window_title, CHAR_PTR* extensions)
	{
		OPENFILENAME fm;
		char selected[MAX_PATH] = "";
		char* filter_string, * result;

		if (!InitLibraries())
		{
			return nullptr;
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

	std::string CreateEscapedString(std::string original)
	{
		std::string result;
		std::string in;
		std::string out;

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
			return nullptr;
		}
		out = result;
		*out = '"';
		++out;
		for (in = original; *in; ++in)
		{
			if (strchr(ESCAPED_CHARS, *in))
			{
				*out = '\\';
				++out;
			}
			*out = *in;
			++out;
		}
		*out = '"';
		++out;
		*out = 0;

		return result;
#undef ESCAPED_CHARS
	}

	// Build list of extensions, like: {"wad","lmp","txt"}
	std::string CreateExtensionsList(CHAR_PTR* extensions)
	{
		char* result, * escaped;
		unsigned result_len;
		unsigned i;

		if (extensions == NULL)
		{
			return nullptr;
		}

		result_len = 3;
		for (i = 0; extensions[i] != NULL; ++i)
		{
			result_len += 5 + strlen(extensions[i]) * 2;
		}

		result = static_cast<decltype(result)>(malloc(result_len));
		if (!result)
		{
			return nullptr;
		}
		StringCopy(result, "{", result_len);

		for (i = 0; extensions[i] != NULL; ++i)
		{
			escaped = CreateEscapedString(extensions[i]);
			if (!escaped)
			{
				free(result);
				return nullptr;
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

	std::string GenerateSelector(std::string const window_title, CHAR_PTR* extensions)
	{
		std::string chooser;
		std::string ext_list = NULL;
		std::string window_title_escaped = NULL;
		std::string result = NULL;
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
				return nullptr;
			}
		}

		// Calculate size.
		if (window_title != NULL)
		{
			window_title_escaped = CreateEscapedString(window_title);
			if (!window_title_escaped)
			{
				free(ext_list);
				return nullptr;
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
			return nullptr;
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

	std::string GenerateAppleScript(std::string window_title, CHAR_PTR* extensions)
	{
		std::string selector, * result;
		size_t result_len;

		selector = GenerateSelector(window_title, extensions);
		if (!selector)
		{
			return nullptr;
		}

		result_len = strlen(APPLESCRIPT_WRAPPER) + strlen(selector);
		result = static_cast<decltype(result)>(malloc(result_len));
		if (!result)
		{
			free(selector);
			return nullptr;
		}

		snprintf(result, result_len, APPLESCRIPT_WRAPPER, selector);
		free(selector);

		return result;
	}

	int CanSelectFiles()
	{
		return 1;
	}

	std::string SelectFile(std::string window_title, CHAR_PTR* extensions)
	{
		char* argv[4];
		std::string result, * applescript;

		applescript = GenerateAppleScript(window_title, extensions);
		if (!applescript)
		{
			return nullptr;
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

	static unsigned NumExtensions(CHAR_PTR* extensions)
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
	std::string ExpandExtension(std::string orig)
	{
		int oldlen, newlen, i;
		char* c, * newext = NULL;

		oldlen = strlen(orig);
		newlen = oldlen * 4; // pathological case: 'w' => '[Ww]'
		newext = static_cast<decltype(newext)>(malloc(newlen + 1));

		if (newext == NULL)
		{
			return nullptr;
		}

		c = newext;
		for (i = 0; i < oldlen; ++i)
		{
			if (isalpha(orig[i]))
			{
				*c = '[';
				++c;
				*c = tolower(orig[i]);
				++c;
				*c = toupper(orig[i]);
				++c;
				*c = ']';
				++c;
			}
			else
			{
				*c = orig[i];
				++c;
			}
		}
		*c = '\0';
		return newext;
	}

	std::string SelectFile(std::string window_title, CHAR_PTR* extensions)
	{
		unsigned i;
		size_t len;
		std::string result;
		char** argv;
		int argc;

		if (!ZenityAvailable())
		{
			return nullptr;
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
				std::string newext = ExpandExtension(extensions[i]);
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

#ifndef _WIN32
	std::string ExecReadOutput(char** argv)
	{
		std::string result;
		int completed;
		int pid, status, result_len;
		int pipefd[2];

		if (pipe(pipefd) != 0)
		{
			return nullptr;
		}

		pid = fork();

		if (pid == 0)
		{
			dup2(pipefd[1], fileno(stdout));
			execv(argv[0], argv);
			exit(-1);
		}

		fcntl(pipefd[0], finalestage_t::F_SETFL, O_NONBLOCK);

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
				std::string new_result = realloc(result, result_len + bytes + 1);
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
};

/**
 * Returns non-zero if a native file selector is available on this platform.
 */
 //int CanSelectFiles();

 /**
  * Open a native file selector to select a file.
  *
  * @param prompt		Pointer to a string containing a prompt to display
  *					in the window.
  * @param extensions NULL-terminated list of filename extensions for
  *					files that can be selected, or @ref DIRECTORY
  *					to select directories.
  */
  //std::string SelectFile(std::string prompt, CHAR_PTR* extensions);

  /**
   * Create a new txt_fileselect_t widget.
   *
   * @param variable	Pointer to a char* variable in which the selected
   *					file should be stored (UTF-8 format).
   * @param size		Width of the file selector widget in characters.
   * @param prompt		Pointer to a string containing a prompt to display
   *					in the file selection window.
   * @param extensions NULL-terminated list of filename extensions that
   *					can be used for this widget, or @ref DIRECTORY
   *					to select directories.
   */
   //txt_fileselect_t* NewFileSelector(char** variable, int size, std::string prompt, CHAR_PTR* extensions);

   /**
	* Special value to use for 'extensions' that selects a directory instead of a file.
	*/
	//extern std::string DIRECTORY[];

} /* END NAMESPACE cudadoom::txt */
