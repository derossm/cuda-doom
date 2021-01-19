/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	Example program: desktop calculator
\**********************************************************************************************************************************************/

#include "../textscreen.h"

enum class operator_t
{
	OP_NONE,
	OP_PLUS,
	OP_MINUS,
	OP_MULT,
	OP_DIV,
};

int starting_input = 0;
int input_value = 0;
txt_label_t* input_box;
int first_operand;
operator_t operator = OP_NONE;

void UpdateInputBox(void)
{
	char buf[20];

	cudadoom::txt::snprintf(buf, sizeof(buf), " %i", input_value);
	cudadoom::txt::SetLabel(input_box, buf);
}

void InsertNumber(cudadoom::txt::UNCAST_ARG(button), cudadoom::txt::UNCAST_ARG(value))
{
	cudadoom::txt::CAST_ARG(int, value);

	if (starting_input)
	{
		input_value = 0;
		starting_input = 0;
	}

	input_value *= 10;
	input_value += *value;
	UpdateInputBox();
}

void AddNumberButton(cudadoom::txt::txt_table_t* table, int value)
{
	int* val_copy = static_cast<decltype(val_copy)>(malloc(sizeof(int)));
	*val_copy = value;

	char buf[10];
	cudadoom::txt::snprintf(buf, sizeof(buf), " %i ", value);

	cudadoom::txt::AddWidget(table, cudadoom::txt::NewButton2(buf, InsertNumber, val_copy));
}

void Operator(cudadoom::txt::UNCAST_ARG(button), cudadoom::txt::UNCAST_ARG(op))
{
	cudadoom::txt::CAST_ARG(operator_t, op);

	first_operand = input_value;
	operator = *op;
	starting_input = 1;
}

void AddOperatorButton(cudadoom::txt::txt_table_t* table, ::std::string label, operator_t op)
{
	operator_t* op_copy = static_cast<decltype(op_copy)>(malloc(sizeof(operator_t)));
	*op_copy = op;

	char buf[10];
	cudadoom::txt::snprintf(buf, sizeof(buf), " %s ", label);

	cudadoom::txt::AddWidget(table, cudadoom::txt::NewButton2(buf, Operator, op_copy));
}

void Calculate(cudadoom::txt::UNCAST_ARG(button), void* unused)
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
	cudadoom::txt::Window* window;
	cudadoom::txt::txt_table_t* table;

	window = cudadoom::txt::NewWindow("Calculator");

	input_box = cudadoom::txt::NewLabel("asdf");
	cudadoom::txt::SetBGColor(input_box, cudadoom::txt::ColorType::black);
	cudadoom::txt::AddWidget(window, input_box);
	cudadoom::txt::AddWidget(window, cudadoom::txt::NewSeparator(NULL));
	cudadoom::txt::AddWidget(window, cudadoom::txt::NewStrut(0, 1));

	table = cudadoom::txt::NewTable(4);
	cudadoom::txt::AddWidget(window, table);
	cudadoom::txt::SetWidgetAlign(table, cudadoom::txt::HORIZ_CENTER);

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
	cudadoom::txt::AddWidget(table, NULL);

	cudadoom::txt::AddWidget(table, cudadoom::txt::NewButton2(" = ", Calculate, NULL));
	AddOperatorButton(table, "/", OP_DIV);

	cudadoom::txt::AddWidget(window, cudadoom::txt::NewStrut(0, 1));
	UpdateInputBox();
}

int main(int argc, char* argv[])
{
	if (!cudadoom::txt::Init())
	{
		fprintf(stderr, "Failed to initialise GUI\n");
		exit(-1);
	}

	cudadoom::txt::SetDesktopTitle("Calculator demo");

	BuildGUI();

	cudadoom::txt::GUIMainLoop();

	cudadoom::txt::Shutdown();

	return 0;
}
