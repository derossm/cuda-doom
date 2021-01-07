/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	Example program: GUI test program
		Demonstrates all the main textscreen widgets in use and shows how a simple textscreen program can be written.
\**********************************************************************************************************************************************/

#include "../textscreen.h"

enum
{
	RADIO_VALUE_BADGER,
	RADIO_VALUE_MUSHROOM,
	RADIO_VALUE_SNAKE
};

// also put some crazy extensions to test the escape function. a"b"c"""dd
std::string extensions[] = {"wad", "lmp", "txt", "a\"b\"c\"\"\"dd", "", "\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"", NULL};
std::string radio_values[] = {"Badger", "Mushroom", "Snake"};
std::string textbox_value;
int numbox_value{0};
int radiobutton_value;
std::string file_path;
std::string dir_path;
txt_label_t* value_label;
Window* firstwin;
int cheesy;

void ClosePwnBox(cudadoom::txt::UNCAST_ARG(widget), cudadoom::txt::UNCAST_ARG(window))
{
	cudadoom::txt::CAST_ARG(cudadoom::txt::Window, window);

	cudadoom::txt::CloseWindow(window);
}

void PwnBox(cudadoom::txt::UNCAST_ARG(widget), void* user_data)
{
	cudadoom::txt::Window* window;
	cudadoom::txt::WindowAction* close_button;

	window = cudadoom::txt::NewWindow("Pwned!");
	cudadoom::txt::AddWidget(window, cudadoom::txt::NewLabel(" BOOM! HEADSHOT! "));

	close_button = cudadoom::txt::NewWindowAction(KEY_ENTER, "Close");
	cudadoom::txt::SignalConnect(close_button, "pressed", ClosePwnBox, window);

	cudadoom::txt::SetWindowAction(window, cudadoom::txt::HORIZ_LEFT, NULL);
	cudadoom::txt::SetWindowAction(window, cudadoom::txt::HORIZ_RIGHT, close_button);
}

void UpdateLabel(cudadoom::txt::UNCAST_ARG(widget), void* user_data)
{
/*
	char buf[40];

	cudadoom::txt::StringCopy(buf, " Current value: ", sizeof(buf));
	if (cheesy)
	{
		cudadoom::txt::StringConcat(buf, "Cheesy ", sizeof(buf));
	}
	cudadoom::txt::StringConcat(buf, radio_values[radiobutton_value], sizeof(buf));
	cudadoom::txt::StringConcat(buf, "\n", sizeof(buf));

	cudadoom::txt::SetLabel(value_label, buf);
/**/
	std::string buf{[&]()
	{
		const char rv[]{" Current value: "};

		if (cheesy)
		{
			return rv + "Cheesy ";
		}
		else
		{
			return rv;
		}
	}() + radio_values[radiobutton_value] + "\n"};

	cudadoom::txt::SetLabel(value_label, buf);
}

void CloseWindow(cudadoom::txt::UNCAST_ARG(button), void* user_data)
{
	cudadoom::txt::CloseWindow(firstwin);
}

void UnicodeWindow(cudadoom::txt::UNCAST_ARG(widget), void* user_data)
{
	static std::string strings[] = {"lunedì", "martedì", "mercoledì", "giovedì", "venerdì", "sabato", "domenica"};

	static int var1;
	static int var2;
	cudadoom::txt::Window* window = cudadoom::txt::NewWindow("Questo è in Italiano");

	cudadoom::txt::AddWidgets(window,
		cudadoom::txt::NewButton("Questo è un tasto"),
		cudadoom::txt::NewCheckBox("Questo è un checkbox", &var1),
		cudadoom::txt::NewDropdownList(&var2, strings, 7),
		cudadoom::txt::NewSeparator("Questo è un separatore"),
		cudadoom::txt::NewLabel("Leggi questo, è pieno di\ninformazioni interessanti"),
		cudadoom::txt::NewRadioButton("Ma questo non è un radio??", &var1, 0),
		NULL);

	cudadoom::txt::SetWindowAction(window, cudadoom::txt::HORIZ_RIGHT, cudadoom::txt::NewWindowAction(KEY_ENTER, "Nullità"));

}

void SetupWindow(void)
{
	cudadoom::txt::Window* window;
	cudadoom::txt::txt_table_t* table;
	cudadoom::txt::txt_table_t* rightpane;
	cudadoom::txt::CheckBox* cheesy_checkbox;
	cudadoom::txt::WindowAction* pwn;
	cudadoom::txt::txt_label_t* toplabel;
	char buf[100];

	window = cudadoom::txt::NewWindow("Window test");

	cudadoom::txt::SetWindowHelpURL(window, "https://www.youtube.com/watch?v=dQw4w9WgXcQ");

	cudadoom::txt::AddWidget(window, cudadoom::txt::NewSeparator("Main section"));
	table = cudadoom::txt::NewTable(3);

	toplabel = cudadoom::txt::NewLabel("This is a multiline label.\nA single label object contains\nall three of these lines.");
	cudadoom::txt::AddWidget(window, toplabel);
	cudadoom::txt::SetWidgetAlign(toplabel, cudadoom::txt::HORIZ_CENTER);

	//cudadoom::txt::AddWidget(window, cudadoom::txt::NewScrollPane(15, 4, table));
	cudadoom::txt::AddWidget(window, table);

	for (size_t i{0}; i < 3; ++i)
	{
		cudadoom::txt::snprintf(buf, sizeof(buf), "Option %i in a table:", i + 1);
		cudadoom::txt::AddWidget(table, cudadoom::txt::NewLabel(buf));
		cudadoom::txt::snprintf(buf, sizeof(buf), " Button %i-1 ", i + 1);
		cudadoom::txt::AddWidget(table, cudadoom::txt::NewButton(buf));
		cudadoom::txt::snprintf(buf, sizeof(buf), " Button %i-2 ", i + 1);
		cudadoom::txt::AddWidget(table, cudadoom::txt::NewButton(buf));
	}

	cudadoom::txt::AddWidgets(table,
		cudadoom::txt::NewLabel("Still the same table, but:\nThis label magically overflows\nacross multiple cells! Cool, huh?"),
		cudadoom::txt::TABLE_OVERFLOW_RIGHT,
		cudadoom::txt::NewButton("Do nothing"),
		cudadoom::txt::TABLE_OVERFLOW_DOWN,
		cudadoom::txt::TABLE_OVERFLOW_DOWN,
		cudadoom::txt::NewButton2("Qualcosa?", UnicodeWindow, NULL),
		NULL);

	cudadoom::txt::AddWidget(window, cudadoom::txt::NewStrut(0, 1));
	value_label = cudadoom::txt::NewLabel("");
	cudadoom::txt::AddWidget(window, value_label);

	table = cudadoom::txt::NewTable(2);
	cudadoom::txt::AddWidget(window, table);
	cudadoom::txt::SetWidgetAlign(table, cudadoom::txt::HORIZ_CENTER);

	cheesy_checkbox = cudadoom::txt::NewCheckBox("Cheesy", &cheesy);
	cudadoom::txt::AddWidget(table, cheesy_checkbox);
	cudadoom::txt::SignalConnect(cheesy_checkbox, "changed", UpdateLabel, NULL);

	rightpane = cudadoom::txt::NewTable(1);
	cudadoom::txt::AddWidget(table, rightpane);

	for (size_t i{0}; i < 3; ++i)
	{
		cudadoom::txt::RadioButton* rbut;

		rbut = cudadoom::txt::NewRadioButton(radio_values[i], &radiobutton_value, i);
		cudadoom::txt::AddWidget(rightpane, rbut);
		cudadoom::txt::SignalConnect(rbut, "selected", UpdateLabel, NULL);
	}

	UpdateLabel(NULL, NULL);

	cudadoom::txt::AddWidget(window, cudadoom::txt::NewButton2("Close Window", CloseWindow, NULL));

	pwn = cudadoom::txt::NewWindowAction(KEY_F1, "PWN!");
	cudadoom::txt::SetWindowAction(window, cudadoom::txt::HORIZ_CENTER, pwn);
	cudadoom::txt::SignalConnect(pwn, "pressed", PwnBox, NULL);

	firstwin = window;
}

void Window2(void)
{
	cudadoom::txt::Window* window;
	cudadoom::txt::txt_table_t* table;
	cudadoom::txt::txt_table_t* unselectable_table;
	cudadoom::txt::txt_scrollpane_t* scrollpane;

	window = cudadoom::txt::NewWindow("Another test");
	cudadoom::txt::SetWindowPosition(window, cudadoom::txt::HORIZ_RIGHT, cudadoom::txt::VERT_TOP, cudadoom::txt::SCREEN_W - 1, 1);

	cudadoom::txt::AddWidgets(window, cudadoom::txt::NewScrollPane(40, 1, cudadoom::txt::NewLabel("* Unselectable scroll pane *")), unselectable_table = cudadoom::txt::NewTable(1), NULL);

	cudadoom::txt::AddWidget(unselectable_table, cudadoom::txt::NewLabel("* Unselectable table *"));
	cudadoom::txt::AddWidget(unselectable_table, cudadoom::txt::NewLabel(
		"This is a UTF-8 string:\n\xc3\x80 bient\xc3\xb4t na\xc3\xaet \xc3\xa9v\xc3\xaaque \xc3\xa0 l'\xc5\x93uvre p\xc3\xa8re."));

	cudadoom::txt::AddWidget(window, cudadoom::txt::NewSeparator("Input boxes"));
	table = cudadoom::txt::NewTable(2);
	cudadoom::txt::AddWidget(window, table);
	cudadoom::txt::AddWidgets(table,
		cudadoom::txt::NewLabel("String: "),
		cudadoom::txt::NewInputBox(&textbox_value, 20),
		cudadoom::txt::NewLabel("Int: "),
		cudadoom::txt::NewIntInputBox(&numbox_value, 10),
		cudadoom::txt::NewLabel("Spin control:"),
		cudadoom::txt::NewSpinControl(&numbox_value, 0, 15),
		cudadoom::txt::NewLabel("File:"),
		cudadoom::txt::NewFileSelector(&file_path, 28, "Select file:", extensions),
		cudadoom::txt::NewLabel("Directory:"),
		cudadoom::txt::NewFileSelector(&dir_path, 28, "Select directory:", cudadoom::txt::DIRECTORY),
		NULL);

	cudadoom::txt::AddWidget(window, cudadoom::txt::NewSeparator("Scroll pane test"));
	scrollpane = cudadoom::txt::NewScrollPane(40, 5, cudadoom::txt::NewLabel(
		"This is a scrollable pane. The contents\nof this box are larger than the box\nitself, but it can be scrolled around\n"
		"to explore the full contents.\n\nScrollable panes can be scrolled both\nvertically and horizontally. They\n"
		"can contain any widget. The scroll bars\nappear automatically as needed.\n\n"
		"This is a very long line of text that forces a horizontal scrollbar"
	));
	cudadoom::txt::AddWidget(window, scrollpane);
}

void ScrollingMenu(void)
{
	cudadoom::txt::Window* window;
	cudadoom::txt::Button* button;
	cudadoom::txt::txt_table_t* table;

	window = cudadoom::txt::NewWindow("Scrollable menu");

	table = cudadoom::txt::NewTable(1);

	cudadoom::txt::AddWidgets(table,
		cudadoom::txt::NewButton("Configure display"),
		cudadoom::txt::NewButton("Configure joystick"),
		cudadoom::txt::NewButton("Configure keyboard"),
		cudadoom::txt::NewButton("Configure mouse"),
		cudadoom::txt::NewButton("Configure sound"),
		cudadoom::txt::NewStrut(0, 1),
		button = cudadoom::txt::NewButton("Save Parameters and launch DOOM"),
		cudadoom::txt::NewStrut(0, 1),
		cudadoom::txt::NewButton("Start a network game"),
		cudadoom::txt::NewButton("Join a network game"),
		cudadoom::txt::NewButton("Multiplayer configuration"),
		NULL);

	cudadoom::txt::SignalConnect(button, "pressed", PwnBox, NULL);

	cudadoom::txt::AddWidget(window, cudadoom::txt::NewScrollPane(0, 6, table));
}

int main(int argc, char* argv[])
{
	if (!cudadoom::txt::Init())
	{
		fprintf(stderr, "Failed to initialise GUI\n");
		exit(-1);
	}

	cudadoom::txt::SetDesktopTitle("Not Chocolate Doom Setup");

	ScrollingMenu();
	Window2();
	SetupWindow();

	cudadoom::txt::GUIMainLoop();

	cudadoom::txt::Shutdown();

	return 0;
}
