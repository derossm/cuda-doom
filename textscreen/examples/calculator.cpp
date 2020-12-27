/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	Example program: desktop calculator
\**********************************************************************************************************************************************/

#include "..\textscreen.h"

typedef enum
{
	OP_NONE,
	OP_PLUS,
	OP_MINUS,
	OP_MULT,
	OP_DIV,
} operator_t;

int starting_input = 0;
int input_value = 0;
txt_label_t *input_box;
int first_operand;
operator_t operator = OP_NONE;

void UpdateInputBox(void)
{
	char buf[20];

	cudadoom::txt::TXT_snprintf(buf, sizeof(buf), " %i", input_value);
	cudadoom::txt::TXT_SetLabel(input_box, buf);
}

void InsertNumber(cudadoom::txt::TXT_UNCAST_ARG(button), cudadoom::txt::TXT_UNCAST_ARG(value))
{
	cudadoom::txt::TXT_CAST_ARG(int, value);

	if (starting_input)
	{
		input_value = 0;
		starting_input = 0;
	}

	input_value *= 10;
	input_value += *value;
	UpdateInputBox();
}

void AddNumberButton(cudadoom::txt::txt_table_t *table, int value)
{
	int* val_copy = static_cast<decltype(val_copy)>(malloc(sizeof(int)));
	*val_copy = value;

	char buf[10];
	cudadoom::txt::TXT_snprintf(buf, sizeof(buf), " %i ", value);

	cudadoom::txt::TXT_AddWidget(table, cudadoom::txt::TXT_NewButton2(buf, InsertNumber, val_copy));
}

void Operator(cudadoom::txt::TXT_UNCAST_ARG(button), cudadoom::txt::TXT_UNCAST_ARG(op))
{
	cudadoom::txt::TXT_CAST_ARG(operator_t, op);

	first_operand = input_value;
	operator = *op;
	starting_input = 1;
}

void AddOperatorButton(cudadoom::txt::txt_table_t* table, const char* label, operator_t op)
{
	operator_t* op_copy = static_cast<decltype(op_copy)>(malloc(sizeof(operator_t)));
	*op_copy = op;

	char buf[10];
	cudadoom::txt::TXT_snprintf(buf, sizeof(buf), " %s ", label);

	cudadoom::txt::TXT_AddWidget(table, cudadoom::txt::TXT_NewButton2(buf, Operator, op_copy));
}

void Calculate(cudadoom::txt::TXT_UNCAST_ARG(button), void *unused)
{
	switch (operator)
	{
		case OP_PLUS:
			input_value = first_operand + input_value;
			break;
		case OP_MINUS:
			input_value = first_operand - input_value;
			break;
		case OP_MULT:
			input_value = first_operand * input_value;
			break;
		case OP_DIV:
			input_value = first_operand / input_value;
			break;
		case OP_NONE:
			break;
	}

	UpdateInputBox();

	operator = OP_NONE;
	starting_input = 1;
}

void BuildGUI()
{
	cudadoom::txt::txt_window_t *window;
	cudadoom::txt::txt_table_t *table;

	window = cudadoom::txt::TXT_NewWindow("Calculator");

	input_box = cudadoom::txt::TXT_NewLabel("asdf");
	cudadoom::txt::TXT_SetBGColor(input_box, cudadoom::txt::txt_color_t::TXT_COLOR_BLACK);
	cudadoom::txt::TXT_AddWidget(window, input_box);
	cudadoom::txt::TXT_AddWidget(window, cudadoom::txt::TXT_NewSeparator(NULL));
	cudadoom::txt::TXT_AddWidget(window, cudadoom::txt::TXT_NewStrut(0, 1));

	table = cudadoom::txt::TXT_NewTable(4);
	cudadoom::txt::TXT_AddWidget(window, table);
	cudadoom::txt::TXT_SetWidgetAlign(table, cudadoom::txt::TXT_HORIZ_CENTER);

	AddNumberButton(table, 7);
	AddNumberButton(table, 8);
	AddNumberButton(table, 9);
	AddOperatorButton(table, "*", OP_MULT);
	AddNumberButton(table, 4);
	AddNumberButton(table, 5);
	AddNumberButton(table, 6);
	AddOperatorButton(table, "-", OP_MINUS);
	AddNumberButton(table, 1);
	AddNumberButton(table, 2);
	AddNumberButton(table, 3);
	AddOperatorButton(table, "+", OP_PLUS);
	AddNumberButton(table, 0);
	cudadoom::txt::TXT_AddWidget(table, NULL);

	cudadoom::txt::TXT_AddWidget(table, cudadoom::txt::TXT_NewButton2(" = ", Calculate, NULL));
	AddOperatorButton(table, "/", OP_DIV);

	cudadoom::txt::TXT_AddWidget(window, cudadoom::txt::TXT_NewStrut(0, 1));
	UpdateInputBox();
}

int main(int argc, char *argv[])
{
	if (!cudadoom::txt::TXT_Init())
	{
		fprintf(stderr, "Failed to initialise GUI\n");
		exit(-1);
	}

	cudadoom::txt::TXT_SetDesktopTitle("Calculator demo");

	BuildGUI();

	cudadoom::txt::TXT_GUIMainLoop();

	cudadoom::txt::TXT_Shutdown();

	return 0;
}
