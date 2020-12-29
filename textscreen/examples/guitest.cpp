/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	Example program: GUI test program
		Demonstrates all the main textscreen widgets in use and shows how a simple textscreen program can be written.
\**********************************************************************************************************************************************/


#include "..\textscreen.h"

enum
{
	RADIO_VALUE_BADGER,
	RADIO_VALUE_MUSHROOM,
	RADIO_VALUE_SNAKE
};

// also put some crazy extensions to test the escape function. a"b"c"""dd
const char* extensions[] = { "wad", "lmp", "txt", "a\"b\"c\"\"\"dd", "", "\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"", NULL };
const char* radio_values[] = { "Badger", "Mushroom", "Snake" };
char* textbox_value{nullptr};
int numbox_value{0};
int radiobutton_value;
char* file_path{nullptr};
char* dir_path{nullptr};
txt_label_t* value_label;
txt_window_t* firstwin;
int cheesy;

void ClosePwnBox(cudadoom::txt::TXT_UNCAST_ARG(widget), cudadoom::txt::TXT_UNCAST_ARG(window))
{
	cudadoom::txt::TXT_CAST_ARG(cudadoom::txt::txt_window_t, window);

	cudadoom::txt::TXT_CloseWindow(window);
}

void PwnBox(cudadoom::txt::TXT_UNCAST_ARG(widget), void* user_data)
{
	cudadoom::txt::txt_window_t* window;
	cudadoom::txt::txt_window_action_t* close_button;

	window = cudadoom::txt::TXT_NewWindow("Pwned!");
	cudadoom::txt::TXT_AddWidget(window, cudadoom::txt::TXT_NewLabel(" BOOM! HEADSHOT! "));

	close_button = cudadoom::txt::TXT_NewWindowAction(KEY_ENTER, "Close");
	cudadoom::txt::TXT_SignalConnect(close_button, "pressed", ClosePwnBox, window);

	cudadoom::txt::TXT_SetWindowAction(window, cudadoom::txt::TXT_HORIZ_LEFT, NULL);
	cudadoom::txt::TXT_SetWindowAction(window, cudadoom::txt::TXT_HORIZ_RIGHT, close_button);
}

void UpdateLabel(cudadoom::txt::TXT_UNCAST_ARG(widget), void* user_data)
{
	char buf[40];

	cudadoom::txt::TXT_StringCopy(buf, " Current value: ", sizeof(buf));
	if (cheesy)
	{
		cudadoom::txt::TXT_StringConcat(buf, "Cheesy ", sizeof(buf));
	}
	cudadoom::txt::TXT_StringConcat(buf, radio_values[radiobutton_value], sizeof(buf));
	cudadoom::txt::TXT_StringConcat(buf, "\n", sizeof(buf));

	cudadoom::txt::TXT_SetLabel(value_label, buf);
}

void CloseWindow(cudadoom::txt::TXT_UNCAST_ARG(button), void* user_data)
{
	cudadoom::txt::TXT_CloseWindow(firstwin);
}

void UnicodeWindow(cudadoom::txt::TXT_UNCAST_ARG(widget), void* user_data)
{
	static const char *strings[] = {"lunedì", "martedì", "mercoledì", "giovedì", "venerdì", "sabato", "domenica"};

	static int var1;
	static int var2;
	cudadoom::txt::txt_window_t* window = cudadoom::txt::TXT_NewWindow("Questo è in Italiano");

	cudadoom::txt::TXT_AddWidgets(window,
					cudadoom::txt::TXT_NewButton("Questo è un tasto"),
					cudadoom::txt::TXT_NewCheckBox("Questo è un checkbox", &var1),
					cudadoom::txt::TXT_NewDropdownList(&var2, strings, 7),
					cudadoom::txt::TXT_NewSeparator("Questo è un separatore"),
					cudadoom::txt::TXT_NewLabel("Leggi questo, è pieno di\ninformazioni interessanti"),
					cudadoom::txt::TXT_NewRadioButton("Ma questo non è un radio??", &var1, 0),
					NULL);

	cudadoom::txt::TXT_SetWindowAction(window, cudadoom::txt::TXT_HORIZ_RIGHT, cudadoom::txt::TXT_NewWindowAction(KEY_ENTER, "Nullità"));

}

void SetupWindow(void)
{
	cudadoom::txt::txt_window_t* window;
	cudadoom::txt::txt_table_t* table;
	cudadoom::txt::txt_table_t* rightpane;
	cudadoom::txt::txt_checkbox_t *cheesy_checkbox;
	cudadoom::txt::txt_window_action_t* pwn;
	cudadoom::txt::txt_label_t* toplabel;
	char buf[100];

	window = cudadoom::txt::TXT_NewWindow("Window test");

	cudadoom::txt::TXT_SetWindowHelpURL(window, "https://www.youtube.com/watch?v=dQw4w9WgXcQ");

	cudadoom::txt::TXT_AddWidget(window, cudadoom::txt::TXT_NewSeparator("Main section"));
	table = cudadoom::txt::TXT_NewTable(3);

	toplabel = cudadoom::txt::TXT_NewLabel("This is a multiline label.\nA single label object contains\nall three of these lines.");
	cudadoom::txt::TXT_AddWidget(window, toplabel);
	cudadoom::txt::TXT_SetWidgetAlign(toplabel, cudadoom::txt::TXT_HORIZ_CENTER);

	//cudadoom::txt::TXT_AddWidget(window, cudadoom::txt::TXT_NewScrollPane(15, 4, table));
	cudadoom::txt::TXT_AddWidget(window, table);

	for (size_t i{0}; i < 3; ++i)
	{
		cudadoom::txt::TXT_snprintf(buf, sizeof(buf), "Option %i in a table:", i + 1);
		cudadoom::txt::TXT_AddWidget(table, cudadoom::txt::TXT_NewLabel(buf));
		cudadoom::txt::TXT_snprintf(buf, sizeof(buf), " Button %i-1 ", i + 1);
		cudadoom::txt::TXT_AddWidget(table, cudadoom::txt::TXT_NewButton(buf));
		cudadoom::txt::TXT_snprintf(buf, sizeof(buf), " Button %i-2 ", i + 1);
		cudadoom::txt::TXT_AddWidget(table, cudadoom::txt::TXT_NewButton(buf));
	}

	cudadoom::txt::TXT_AddWidgets(table,
					cudadoom::txt::TXT_NewLabel("Still the same table, but:\nThis label magically overflows\nacross multiple cells! Cool, huh?"),
					cudadoom::txt::TXT_TABLE_OVERFLOW_RIGHT,
					cudadoom::txt::TXT_NewButton("Do nothing"),
					cudadoom::txt::TXT_TABLE_OVERFLOW_DOWN,
					cudadoom::txt::TXT_TABLE_OVERFLOW_DOWN,
					cudadoom::txt::TXT_NewButton2("Qualcosa?", UnicodeWindow, NULL),
					NULL);

	cudadoom::txt::TXT_AddWidget(window, cudadoom::txt::TXT_NewStrut(0, 1));
	value_label = cudadoom::txt::TXT_NewLabel("");
	cudadoom::txt::TXT_AddWidget(window, value_label);

	table = cudadoom::txt::TXT_NewTable(2);
	cudadoom::txt::TXT_AddWidget(window, table);
	cudadoom::txt::TXT_SetWidgetAlign(table, cudadoom::txt::TXT_HORIZ_CENTER);

	cheesy_checkbox = cudadoom::txt::TXT_NewCheckBox("Cheesy", &cheesy);
	cudadoom::txt::TXT_AddWidget(table, cheesy_checkbox);
	cudadoom::txt::TXT_SignalConnect(cheesy_checkbox, "changed", UpdateLabel, NULL);

	rightpane = cudadoom::txt::TXT_NewTable(1);
	cudadoom::txt::TXT_AddWidget(table, rightpane);

	for (size_t i{0}; i<3; ++i)
	{
		cudadoom::txt::txt_radiobutton_t* rbut;

		rbut = cudadoom::txt::TXT_NewRadioButton(radio_values[i], &radiobutton_value, i);
		cudadoom::txt::TXT_AddWidget(rightpane, rbut);
		cudadoom::txt::TXT_SignalConnect(rbut, "selected", UpdateLabel, NULL);
	}

	UpdateLabel(NULL, NULL);

	cudadoom::txt::TXT_AddWidget(window, cudadoom::txt::TXT_NewButton2("Close Window", CloseWindow, NULL));

	pwn = cudadoom::txt::TXT_NewWindowAction(KEY_F1, "PWN!");
	cudadoom::txt::TXT_SetWindowAction(window, cudadoom::txt::TXT_HORIZ_CENTER, pwn);
	cudadoom::txt::TXT_SignalConnect(pwn, "pressed", PwnBox, NULL);

	firstwin = window;
}

void Window2(void)
{
	cudadoom::txt::txt_window_t* window;
	cudadoom::txt::txt_table_t* table;
	cudadoom::txt::txt_table_t* unselectable_table;
	cudadoom::txt::txt_scrollpane_t* scrollpane;

	window = cudadoom::txt::TXT_NewWindow("Another test");
	cudadoom::txt::TXT_SetWindowPosition(window, cudadoom::txt::TXT_HORIZ_RIGHT, cudadoom::txt::TXT_VERT_TOP, cudadoom::txt::TXT_SCREEN_W - 1, 1);

	cudadoom::txt::TXT_AddWidgets(window, cudadoom::txt::TXT_NewScrollPane(40, 1, cudadoom::txt::TXT_NewLabel("* Unselectable scroll pane *")), unselectable_table = cudadoom::txt::TXT_NewTable(1), NULL);

	cudadoom::txt::TXT_AddWidget(unselectable_table, cudadoom::txt::TXT_NewLabel("* Unselectable table *"));
	cudadoom::txt::TXT_AddWidget(unselectable_table, cudadoom::txt::TXT_NewLabel(
		"This is a UTF-8 string:\n\xc3\x80 bient\xc3\xb4t na\xc3\xaet \xc3\xa9v\xc3\xaaque \xc3\xa0 l'\xc5\x93uvre p\xc3\xa8re."));

	cudadoom::txt::TXT_AddWidget(window, cudadoom::txt::TXT_NewSeparator("Input boxes"));
	table = cudadoom::txt::TXT_NewTable(2);
	cudadoom::txt::TXT_AddWidget(window, table);
	cudadoom::txt::TXT_AddWidgets(table,
					cudadoom::txt::TXT_NewLabel("String: "),
					cudadoom::txt::TXT_NewInputBox(&textbox_value, 20),
					cudadoom::txt::TXT_NewLabel("Int: "),
					cudadoom::txt::TXT_NewIntInputBox(&numbox_value, 10),
					cudadoom::txt::TXT_NewLabel("Spin control:"),
					cudadoom::txt::TXT_NewSpinControl(&numbox_value, 0, 15),
					cudadoom::txt::TXT_NewLabel("File:"),
					cudadoom::txt::TXT_NewFileSelector(&file_path, 28, "Select file:", extensions),
					cudadoom::txt::TXT_NewLabel("Directory:"),
					cudadoom::txt::TXT_NewFileSelector(&dir_path, 28, "Select directory:", cudadoom::txt::TXT_DIRECTORY),
					NULL);

	cudadoom::txt::TXT_AddWidget(window, cudadoom::txt::TXT_NewSeparator("Scroll pane test"));
	scrollpane = cudadoom::txt::TXT_NewScrollPane(40, 5, cudadoom::txt::TXT_NewLabel(
		"This is a scrollable pane. The contents\nof this box are larger than the box\nitself, but it can be scrolled around\n"
		"to explore the full contents.\n\nScrollable panes can be scrolled both\nvertically and horizontally. They\n"
		"can contain any widget. The scroll bars\nappear automatically as needed.\n\n"
		"This is a very long line of text that forces a horizontal scrollbar"
	));
	cudadoom::txt::TXT_AddWidget(window, scrollpane);
}

void ScrollingMenu(void)
{
	cudadoom::txt::txt_window_t *window;
	cudadoom::txt::txt_button_t *button;
	cudadoom::txt::txt_table_t *table;

	window = cudadoom::txt::TXT_NewWindow("Scrollable menu");

	table = cudadoom::txt::TXT_NewTable(1);

	cudadoom::txt::TXT_AddWidgets(table,
					cudadoom::txt::TXT_NewButton("Configure display"),
					cudadoom::txt::TXT_NewButton("Configure joystick"),
					cudadoom::txt::TXT_NewButton("Configure keyboard"),
					cudadoom::txt::TXT_NewButton("Configure mouse"),
					cudadoom::txt::TXT_NewButton("Configure sound"),
					cudadoom::txt::TXT_NewStrut(0, 1),
					button = cudadoom::txt::TXT_NewButton("Save Parameters and launch DOOM"),
					cudadoom::txt::TXT_NewStrut(0, 1),
					cudadoom::txt::TXT_NewButton("Start a network game"),
					cudadoom::txt::TXT_NewButton("Join a network game"),
					cudadoom::txt::TXT_NewButton("Multiplayer configuration"),
					NULL);

	cudadoom::txt::TXT_SignalConnect(button, "pressed", PwnBox, NULL);

	cudadoom::txt::TXT_AddWidget(window, cudadoom::txt::TXT_NewScrollPane(0, 6, table));
}

int main(int argc, char *argv[])
{
	if (!cudadoom::txt::TXT_Init())
	{
		fprintf(stderr, "Failed to initialise GUI\n");
		exit(-1);
	}

	cudadoom::txt::TXT_SetDesktopTitle("Not Chocolate Doom Setup");

	ScrollingMenu();
	Window2();
	SetupWindow();

	cudadoom::txt::TXT_GUIMainLoop();

	cudadoom::txt::TXT_Shutdown();

	return 0;
}
