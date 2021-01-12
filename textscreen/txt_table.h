/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h
#include "../derma/keybinds.h
#include "txt_common.h"
#include "txt_widget.h"

namespace cudadoom::txt
{

/**
	Table widget.

	A table is a widget that contains other widgets. It may have multiple columns, in which case the child widgets are laid out
	in a grid. Columns automatically grow as necessary, although minimum column widths can be set using @ref SetColumnWidths.

	To create a new table, use @ref NewTable. It is also possible to use @ref MakeHorizontalTable to create a table, specifying
	widgets to place inside a horizontal list. A vertical list is possible simply by creating a table containing a single column.
 */

// txt_table_overflow_right;
// txt_table_overflow_down;
// txt_table_eol;
// txt_table_empty;

template<typename T>
class TableBase : public WidgetBase<T>
{
public:
	// Widgets in this table
	// The widget at (x,y) in the table is widgets[columns * y + x]
	//::std::vector<::std::unique_ptr<Widget>> widgets;

	int columns;

	// Currently selected:
	int selected_x{0};
	int selected_y{0};

public:

	TableBase(int _columns = 1) //: widget_class<Table>{}, columns{_columns},
		//widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy}
	{
		// Add a strut for each column at the start of the table. These are used by the SetColumnWidths function below:
		// the struts are created with widths of 0 each, but this function changes them.
		//for (size_t i{0}; i < columns; ++i)
		{
			//AddWidget(NewStrut(0, 0));
		}
	}

	virtual ~TableBase() {}

	// Determine whether the table is selectable.
	inline bool Selectable() const noexcept override
	{
/*
		// Is the currently-selected cell selectable?
		if (SelectableCell(selected_x, selected_y))
		{
			return true;
		}

		// Find the first selectable cell and set selected_x, selected_y.
		for (size_t i{0}; i < widgets.size(); ++i)
		{
			if (widgets[i]->IsActualWidget() && widgets[i]->SelectableWidget())
			{
				ChangeSelection(i % columns, i / columns);
				return true;
			}
		}
/**/
		// No selectable widgets exist within the table.
		return false;
	}

	inline void CalculateSize() noexcept override
	{
/*
		auto rows = Rows();

		unsigned* row_heights = static_cast<decltype(row_heights)>(malloc(sizeof(int) * rows));
		unsigned* column_widths = static_cast<decltype(column_widths)>(malloc(sizeof(int) * table->columns));

		CalcRowColSizes(row_heights, column_widths);

		width = 0;

		for (size_t x{0}; x < columns; ++x)
		{
			width += column_widths[x];
		}

		height = 0;

		for (size_t y{0}; y < rows; ++y)
		{
			height += row_heights[y];
		}

		free(row_heights);
		free(column_widths);
*/
	}

	inline void Draw() noexcept override
	{
/*
		Widget* widget;
		int i;

		// Check the table's current selection points at something valid before drawing.

		CheckValidSelection(table);

		// Draw all cells
		for (i = 0; i < table->num_widgets; ++i)
		{
			widget = table->widgets[i];

			if (IsActualWidget(widget))
			{
				GotoXY(widget->x, widget->y);
				DrawWidget(widget);
			}
		}
/**/
	}

	inline bool KeyPress(Keys key) noexcept override
	{
/*
		auto rows{Rows(table)};

		// Send to the currently selected widget first
		auto selected{selected_y * columns + selected_x};

		if (selected >= 0 && selected < widgets.size())
		{
			if (widgets[selected]->IsActualWidget() && widgets[selected]->SelectableWidget() && widgets[selected]->WidgetKeyPress(key))
			{
				return true;
			}
		}

		if (key == KEY_TAB)
		{
			auto dir{GetModifierState(ModifierType::shift) ? -1 : 1};

			// Cycle through all widgets until we find one that can be selected.
			for (size_t i{selected_y * columns + selected_x + dir}; i >= 0 && i < widgets.size(); i += dir)
			{
				if (widgets[i]->IsActualWidget() && widgets[i]->SelectableWidget())
				{
					ChangeSelection(i % columns, i / columns);
					return true;
				}
			}

			return false;
		}

		if (key == KEY_DOWNARROW)
		{
			// Move cursor down to the next selectable widget
			for (size_t new_y{selected_y + 1}; new_y < rows; ++new_y)
			{
				auto new_x{FindSelectableColumn(new_y, selected_x)};

				if (new_x >= 0)
				{
					// Found a selectable widget in this column!
					ChangeSelection(new_x, new_y);

					return true;
				}
			}
		}

		if (key == KEY_UPARROW)
		{
			// Move cursor up to the next selectable widget
			for (size_t new_y{selected_y - 1}; new_y >= 0; --new_y)
			{
				auto new_x{FindSelectableColumn(new_y, selected_x)};

				if (new_x >= 0)
				{
					// Found a selectable widget in this column!
					ChangeSelection(new_x, new_y);

					return true;
				}
			}
		}

		if (key == KEY_LEFTARROW)
		{
			// Move cursor left
			for (size_t new_x{selected_x - 1}; new_x >= 0; --new_x)
			{
				if (SelectableCell(new_x, selected_y))
				{
					// Found a selectable widget!
					ChangeSelection(new_x, selected_y);

					return true;
				}
			}
		}

		if (key == KEY_RIGHTARROW)
		{
			// Move cursor left
			for (size_t new_x{selected_x + 1}; new_x < columns; ++new_x)
			{
				if (SelectableCell(new_x, selected_y))
				{
					// Found a selectable widget!
					ChangeSelection(new_x, selected_y);

					return true;
				}
			}
		}
/**/
		return false;
	}

	// Responds to mouse presses
	inline bool MousePress(MouseEvent evt) noexcept override
	{
/*
		auto [button, x, y] = evt;
		for (size_t i{0}; i < table->num_widgets; ++i)
		{
			auto widget = table->widgets[i];

			// NULL widgets are spacers
			if (IsActualWidget(widget))
			{
				if (x >= widget->x && x < (int)(widget->x + widget->width) && y >= widget->y && y < (int)(widget->y + widget->height))
				{
					// This is the widget that was clicked!

					// Select the cell if the widget is selectable
					if (SelectableWidget(widget))
					{
						ChangeSelection(table, i % table->columns, i / table->columns);
					}

					// Propagate click
					Widget::MousePress(evt);

					break;
				}
			}
		}
/**/
		return false;
	}

	inline void SetLayout() noexcept override
	{
/*
		// Work out the column widths and row heights
		auto rows{Rows()};

		unsigned* column_widths = static_cast<decltype(column_widths)>(malloc(sizeof(int) * columns));
		unsigned* row_heights = static_cast<decltype(row_heights)>(malloc(sizeof(int) * rows));

		CalcRowColSizes(row_heights, column_widths);

		// If this table only has one column, expand column size to fit the display width. Ensures that separators reach the window edges when drawing windows.
		if (columns == 1)
		{
			column_widths[0] = width;
		}

		// Draw all cells
		auto draw_y{y};

		for (size_t y{0}; y < rows; ++y)
		{
			auto draw_x{x};

			for (size_t x{0}; x < columns; ++x)
			{
				auto i{y * columns + x};

				if (i >= widgets.size())
				{
					break;
				}

				auto widget{widgets[i]};

				if (widget->IsActualWidget())
				{
					CalculateWidgetDimensions(x, y, column_widths, row_heights, &widget->width, &widget->height);
					LayoutCell(x, y, draw_x, draw_y);
				}

				draw_x += column_widths[x];
			}

			draw_y += row_heights[y];
		}

		free(row_heights);
		free(column_widths);
/**/
	}

	inline void SetFocus(bool state) noexcept override
	{

	}

	inline void Destroy() noexcept override
	{
		//ClearTable();
	}
/*
	// Returns true if the given widget in the table's widgets[] array refers to an actual widget - not NULL, or one of the special overflow pointers.
	int IsActualWidget()
	{
		return this != &txt_table_overflow_right && this != &txt_table_overflow_down;
	}

	// Remove all entries from a table
	void ClearTable()
	{
		// Free all widgets
		// Skip over the first (num_columns) widgets in the array, as these are the column struts used to control column width
		for (size_t i{columns}; i < widgets.size(); ++i)
		{
			if (IsActualWidget(widgets[i]))
			{
				widgets.erase(widgets.at(i));
			}
		}
	}

	int Rows()
	{
		return (widgets.size() + columns - 1) / columns;
	}

	// Most widgets occupy just one cell of a table, but if the special overflow constants are used, they can occupy multiple cells.
	// This function figures out for a widget in a given cell, which cells it should actually occupy (always a rectangle).
	void CellOverflowedSize(int x, int y, int* w, int* h)
	{
		if (!widgets[y * columns + x]->IsActualWidget())
		{
			*w = 0;
			*h = 0;
			return;
		}

		*w = columns - x;
		*h = 0;
		for (size_t y1{y}; y1 < Rows(); ++y1)
		{
			// Every overflow cell must point to either (x, y) or another overflow cell. This means the first in every row must be txt_table_overflow_down.
			if (y1 * columns + x >= widgets.size())
			{
				break;
			}

			auto widget{widgets[y1 * table->columns + x]};

			if (y1 != y && widget != &txt_table_overflow_down)
			{
				break;
			}

			for (size_t x1{x + 1}; x1 < x + *w; ++x1)
			{
				if (y1 * columns + x1 >= widgets.size())
				{
					break;
				}

				// Can be either type of overflow, except on the first row. Otherwise we impose a limit on the width.
				widget = widgets[y1 * columns + x1];
				if (widget != &txt_table_overflow_right && (widget != &txt_table_overflow_down || y1 == y))
				{
					*w = x1 - x;
					break;
				}
			}

			//++*h;
			++(*h);
		}
	}

	int IsOverflowingCell(int x, int y)
	{
		int w;
		int h;
		CellOverflowedSize(x, y, &w, &h);
		return w > 1 || h > 1;
	}

	// Using the given column/row size tables, calculate the size of the given widget, storing the result in (w, h).
	void CalculateWidgetDimensions(int x, int y, unsigned* column_widths, unsigned* row_heights, unsigned* w, unsigned* h)
	{
		int cell_w;
		int cell_h;

		// Find which cells this widget occupies.
		CellOverflowedSize(x, y, &cell_w, &cell_h);

		// Add up column / row widths / heights to get the actual dimensions.
		*w = 0;
		for (size_t x1{x}; x1 < x + cell_w; ++x1)
		{
			*w += column_widths[x1];
		}

		*h = 0;
		for (size_t y1{y}; y1 < y + cell_h; ++y1)
		{
			*h += row_heights[y1];
		}
	}

	void CalcRowColSizes(unsigned* row_heights, unsigned* col_widths)
	{
		auto rows{Rows(table)};
		memset(col_widths, 0, sizeof(int) * columns);

		for (size_t y{0}; y < rows; ++y)
		{
			row_heights[y] = 0;

			for (size_t x{0}; x < columns; ++x)
			{
				if (y * columns + x >= widgets.size())
				{
					break;
				}

				auto widget = widgets[y * columns + x];

				if (IsActualWidget())
				{
					CalcWidgetSize();
				}

				// In the first pass we ignore overflowing cells.
				if (IsOverflowingCell(x, y))
				{
					continue;
				}

				// NULL represents an empty spacer
				if (IsActualWidget())
				{
					if (height > row_heights[y])
					{
						row_heights[y] = height;
					}
					if (width > col_widths[x])
					{
						col_widths[x] = width;
					}
				}
			}
		}

		// In the second pass, we go through again and process overflowing widgets, to ensure that they will fit.
		for (size_t y{0}; y < rows; ++y)
		{
			for (size_t x{0}; x < columns; ++x)
			{

				if (y * columns + x >= widgets.size())
				{
					break;
				}

				auto widget = widgets[y * columns + x];
				if (!IsActualWidget())
				{
					continue;
				}

				unsigned w;
				unsigned h;
				// Expand column width and row heights as needed.
				CalculateWidgetDimensions(x, y, col_widths, row_heights, &w, &h);
				if (w < width)
				{
					col_widths[x] += width - w;
				}
				if (h < height)
				{
					row_heights[y] += height - h;
				}
			}
		}
	}

	void FillRowToEnd()
	{
		while ((widgets.size() % columns) != 0)
		{
			AddWidget(&txt_table_overflow_right);
		}
	}

	void AddWidget(::std::unique_ptr<Widget> widget)
	{
		// Convenience alias for NULL:
		// FIXME these are impossible conditions now
		if (widget.get() == &txt_table_empty)
		{
			widget = nullptr;
		}
		else if (widget.get() == &txt_table_eol)
		{
			FillRowToEnd();
			return;
		}

		// We have special handling for the separator widget:
		auto is_separator{IsActualWidget() && widget_class == &txt_separator_class};

		// If we add two separators consecutively, the new separator replaces the first. This allows us to override the "implicit" separator that is
		// added at the top of a window when it is created.
		if (is_separator)
		{
			for (size_t i{widgets.size() - 1}; i >= 0; --i)
			{
				auto last_widget{widgets[i]};

				if (last_widget->IsActualWidget() && widget_class == &txt_separator_class && last_widget->widget_class == &txt_separator_class)
				{
					widgets[i] = widget;
					return;
				}
				else if (last_widget != &txt_table_overflow_right)
				{
					break;
				}
			}
		}

		// Separators begin on a new line.
		if (is_separator)
		{
			FillRowToEnd();
		}

		//widgets = realloc(widgets, sizeof(Widget*) * (widgets.size() + 1));
		//widgets[num_widgets] = widget;
		//++(num_widgets);
		widgets.push_back(widget);

		// Maintain parent pointer.
		if (IsActualWidget())
		{
			widget->parent = this;
		}

		// Separators always take up the entire line.
		if (is_separator)
		{
			FillRowToEnd();
		}
	}

	// Add multiple widgets to a table.
	void AddWidgets(Table* table, ...)
	{
		va_list args;
		va_start(args, table);

		// Keep adding widgets until a NULL is reached.
		for (;;)
		{
			// FIXME
			::std::unique_ptr<Widget> widget = va_arg(args, ::std::unique_ptr<Widget>);

			if (widget == nullptr)
			{
				break;
			}

			AddWidget(widget);
		}

		va_end(args);
	}

	bool SelectableCell(int x, int y)
	{
		if (x < 0 || x >= columns)
		{
			return false;
		}

		if (auto i{y * columns + x}; i >= 0 && i < widgets.size())
		{
			auto widget = widgets[i].get();
			return widget->IsActualWidget() && widget->SelectableWidget() && widget->visible();
		}

		return false;
	}

	// Tries to locate a selectable widget in the given row, returning the column number of the first column available or -1 if none are
	// available in the given row. Starts from start_col, then searches nearby columns.
	int FindSelectableColumn(int row, int start_col)
	{
		for (size_t x{0}; x < columns; ++x)
		{
			// Search to the right
			if (SelectableCell(start_col + x, row))
			{
				return start_col + x;
			}

			// Search to the left
			if (SelectableCell(start_col - x, row))
			{
				return start_col - x;
			}
		}

		// None available
		return -1;
	}

	// Change the selected widget.
	void ChangeSelection(int x, int y)
	{
		// No change?
		if (x == selected_x && y == selected_y)
		{
			return;
		}

		// Unfocus current widget:
		if (auto i{selected_y * columns + selected_x}; i < widgets.size())
		{
			auto cur_widget{widgets[i]};

			if (focused() && cur_widget->IsActualWidget())
			{
				cur_widget->SetWidgetFocus(false);
			}
		}

		// Focus new widget.
		auto new_widget{widgets[y * columns + x]};

		table->selected_x = x;
		table->selected_y = y;

		if (focused() && new_widget)
		{
			new_widget->SetWidgetFocus(true);
		}
	}

	// Check the currently selected widget in the table is valid.
	void CheckValidSelection()
	{
		auto rows{Rows()};

		for (size_t new_y{selected_y}; new_y < rows; ++new_y)
		{
			auto new_x{FindSelectableColumn(new_y, table->selected_x)};

			if (new_x >= 0)
			{
				// Found a selectable column.
				ChangeSelection(new_x, new_y);

				break;
			}
		}
	}

	void LayoutCell(int x, int y, int draw_x, int draw_y)
	{
		auto widget{widgets[y * columns + x]};

		auto col_width{width};

		// Adjust x position based on alignment property
		switch (align)
		{
		case AlignHorizontal::left:
			width = col_width;
			break;

		case AlignHorizontal::center:
			CalcWidgetSize();

			// Separators are always drawn left-aligned.
			if (widget_class != &txt_separator_class)
			{
				draw_x += (col_width - width) / 2;
			}

			break;

		case AlignHorizontal::right:
			CalcWidgetSize();

			if (widget_class != &txt_separator_class)
			{
				draw_x += col_width - width;
			}
			break;
		}

		// Set the position for this widget
		x = draw_x;
		y = draw_y;

		// Recursively lay out any widgets contained in the widget
		LayoutWidget();
	}

	// Need to pass through focus changes to the selected child widget.
	void Focused(int focused)
	{
		if (auto i{selected_y * columns + selected_x}; i < widgets.size())
		{
			if (widgets[i]->IsActualWidget())
			{
				widgets[i]->SetWidgetFocus(focused);
			}
		}
	}

	// Get the currently-selected widget in a table, recursively searching through sub-tables if necessary.
	Widget* GetSelectedWidget()
	{
		auto index{selected_y * columns + selected_x};

		Widget* result = nullptr;

		if (index >= 0 && index < num_widgets)
		{
			result = widgets[index];

			if (!IsActualWidget(result))
			{
				result = nullptr;
			}
		}

		if (result && result->widget_class == &txt_table_class)
		{
			// NOTE: RECURSION
			result = GetSelectedWidget();
		}

		return result;
	}

	// Selects a given widget in a table, recursively searching any tables within this table. Returns 1 if successful, 0 if unsuccessful.
	int SelectWidget(Widget* widget)
	{
		for (size_t i{0}; i < widgets.size(); ++i)
		{
			if (!widgets[i]->IsActualWidget())
			{
				continue;
			}

			if (widgets[i] == widget)
			{
				// Found the item! Select it and return.
				ChangeSelection(i % columns, i / columns);

				return 1;
			}

			if (widgets[i]->widget_class == &txt_table_class)
			{
				// This item is a subtable. Recursively search this table.
				if (SelectWidget(widgets[i], widget))
				{
					// Found it in the subtable. Select this subtable and return.
					ChangeSelection(i % columns, i / columns);

					return 1;
				}
			}
		}

		// Not found.
		return 0;
	}

	void SetTableColumns(int new_columns)
	{
		// We need as many full rows as are in the current list, plus the remainder from the last row.
		auto new_num_widgets{(widgets.size() / columns) * new_columns + (widgets.size() % columns)};
		decltype(widgets) new_widgets(new_num_widgets);

		// Reset and add one by one from the old table.
		new_num_widgets = 0;

		for (size_t i{0}; i < widgets.size(); ++i)
		{
			auto widget{widgets[i]};
			auto x{i % columns};

			if (x < new_columns)
			{
				new_widgets.push_back(widget);
				++new_num_widgets;
			}
			else if (widget->IsActualWidget())
			{
				widget->DestroyWidget();
			}

			// When we reach the last column of a row, we must pad it out with
			// extra widgets to reach the next row.
			if (x == columns - 1)
			{
				for (size_t j{columns}; j < new_columns; ++j)
				{
					// First row? We need to add struts that are used to apply
					// the column widths.
					if (i < columns)
					{
						widget = &NewStrut(0, 0)->widget;
					}
					else
					{
						widget = &txt_table_overflow_right;
					}
					new_widgets[new_num_widgets] = widget;
					++new_num_widgets;
				}
			}

		}

		widgets.reset();
		widgets = new_widgets;
		table->num_widgets = new_num_widgets;
		table->columns = new_columns;
	}

	// Sets the widths of columns in a table.
	void SetColumnWidths(Table* table, ...)
	{
		va_list args;
		// FIXME take out this table param, but I have no idea how va_list/va_start works
		va_start(args, _NAME(table));

		for (size_t i{0}; i < columns; ++i)
		{
			auto width{va_arg(args, int)};

			auto strut{(txt_strut_t*)widgets[i]};
			strut->width = width;
		}

		va_end(args);
	}

	// Moves the select by at least the given number of characters. Currently quietly ignores pagex, as we don't use it.
	bool PageTable(int pagex, int pagey)
	{
		auto changed{false};

		auto rows{Rows()};

		unsigned* row_heights{static_cast<decltype(row_heights)>(malloc(sizeof(int) * rows))};
		unsigned* column_widths{static_cast<decltype(column_widths)>(malloc(sizeof(int) * columns))};

		CalcRowColSizes(row_heights, column_widths);

		if (pagex)
		{
			// @todo Jump selection to the left or right as needed
		}

		if (pagey)
		{
			int dir;

			// What direction are we moving?
			if (pagey > 0)
			{
				dir = 1;
			}
			else
			{
				dir = -1;
			}

			// Move the cursor until the desired distance is reached.

			auto new_y{selected_y};
			int distance{0};
			while (new_y >= 0 && new_y < rows)
			{
				// We are about to travel a distance equal to the height of the row; we are about to leave.
				distance += row_heights[new_y];

				// *Now* increment the loop.
				new_y += dir;

				auto new_x{FindSelectableColumn(new_y, selected_x)};

				if (new_x >= 0)
				{
					// Found a selectable widget in this column! Select it anyway in case we don't find something better.
					ChangeSelection(new_x, new_y);
					changed = true;

					// ...but is it far enough away?
					if (distance >= abs(pagey))
					{
						break;
					}
				}
			}
		}

		free(row_heights);
		free(column_widths);

		return changed;
	}
/**/
};

class Table : public TableBase<Table>
{

};

/*
// Alternative to NewTable() that allows a list of widgets to be provided in its arguments.
::std::unique_ptr<Table> MakeTable(int columns, ...)
{
	va_list args;

	auto table{::std::make_unique(Table(columns))};
	va_start(args, columns);

	for (;;)
	{
		Widget* widget{va_arg(args, Widget*)};

		if (!widget)
		{
			break;
		}

		table->AddWidget(widget);
	}

	va_end(args);

	return table;
}

// Create a horizontal table from a list of widgets.
::std::unique_ptr<Table> MakeHorizontalTable(Widget* first_widget, ...)
{
	va_list args;
	// First, find the number of arguments to determine the width of the box.
	va_start(args, _NAME(first_widget));

	auto num_args{1};
	for (;;)
	{
		Widget* widget{va_arg(args, Widget*)};

		if (widget == nullptr)
		{
			// End of list
			break;
		}
		else
		{
			++num_args;
		}
	}

	va_end(args);

	// Create the table.
	auto result{::std::make_unique(Table(num_args))};
	result->AddWidget(first_widget);

	// Go through the list again and add each widget.
	va_start(args, _NAME(first_widget));

	for (;;)
	{
		Widget* widget = va_arg(args, Widget*);

		if (widget == nullptr)
		{
			// End of list
			break;
		}
		else
		{
			result->AddWidget(widget);
		}
	}

	va_end(args);

	return result;

}
/**/
//void InitTable(txt_table_t* table, int columns);

/**
* Create a new table.
* @param columns		The number of columns in the new table.
* @return				Pointer to the new table structure.
*/
//txt_table_t* NewTable(int columns);

/**
* Create a new table and populate it with provided widgets.
* The arguments to this function are variable. Each argument must be a pointer to a widget, and the list is terminated with a NULL.
* @param columns		The number of columns in the new table.
* @return				Pointer to the new table structure.
*/
//txt_table_t* MakeTable(int columns, ...);

/**
* Create a table containing the specified widgets packed horizontally, from left to right.
* The arguments to this function are variable. Each argument must be a pointer to a widget, and the list is terminated with a NULL.
* @return				Pointer to the new table structure.
*/
//txt_table_t* MakeHorizontalTable((first_widget), ...);

/**
* Get the currently selected widget within a table. This function will recurse through subtables if necessary.
* @param table			The table.
* @return				Pointer to the widget that is currently selected.
*/
//Widget* GetSelectedWidget((table));

/**
* Add a widget to a table.
* Widgets are added to tables horizontally, from left to right. For example, for a table with three columns, the first call
* to this function will add a widget to the first column, the second call to the second column, the third call to the third
* column, and the fourth will return to the first column, starting a new row.
* For adding many widgets, it may be easier to use @ref AddWidgets.
* @param table			The table.
* @param widget			The widget to add.
*/
//void AddWidget((table), (widget));

/**
* Add multiple widgets to a table. Widgets are added as described in the documentation for the @ref AddWidget function. This function adds
* multiple widgets. The number of arguments is variable, and the argument list must be terminated by a NULL pointer.
* @param table			The table.
*/
//void AddWidgets((table), ...);

/**
 * Select the given widget that is contained within the specified table. This function will recursively search through subtables if necessary.
 * @param table			The table.
 * @param widget		The widget to select.
 * @return				Non-zero (true) if it has been selected, or zero (false) if it was not found within this table.
*/
//int SelectWidget((table), (widget));

/**
* Change the number of columns in the table. Existing widgets in the table will be preserved, unless the change reduces the number of columns,
* in which case the widgets from the 'deleted' columns will be freed. This function can be useful for changing the number of columns in a window,
* which by default are tables containing a single column.
* @param table			The table.
* @param new_columns	The new number of columns.
*/
//void SetTableColumns((table), int new_columns);

/**
* Set the widths of the columns of the table. The arguments to this function are variable, and correspond to the number of columns in the table.
* For example, if a table has five columns, the width of each of the five columns must be specified. The width values are in number of characters.
* Note that this function only sets the minimum widths for columns; if the columns contain widgets that are wider than the widths specified, they will be larger.
* @param table			The table.
*/
//void SetColumnWidths((table), ...);

/**
* Remove all widgets from a table.
* @param table			The table.
*/
//void ClearTable((table));

/**
* Hack to move the selection in a table by a 'page', triggered by the scrollpane. This acts as per the keyboard events for the arrows, but moves
* the selection by at least the specified number of characters.
* @param table			The table.
* @param pagex			Minimum distance to move the selection horizontally.
* @param pagey			Minimum distance to move the selection vertically.
* @return				Non-zero if the selection has been changed.
*/
//int PageTable((table), int pagex, int pagey);

} // end namespace cudadoom::txt
