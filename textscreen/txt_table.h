/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

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
	::std::vector<::std::unique_ptr<WidgetBase<T>>> widgets;

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

		// No selectable widgets exist within the table.
		return false;
	}

	inline void CalculateSize() noexcept override
	{
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
	}

	inline void Draw() noexcept override
	{
		Widget* widget;

		// Check the table's current selection points at something valid before drawing.
		//CheckValidSelection(table);

		// Draw all cells
		for (size_t i{0}; i < table->num_widgets; ++i)
		{
			widget = table->widgets[i];

			if (IsActualWidget(widget))
			{
				GotoXY(widget->x, widget->y);
				DrawWidget(widget);
			}
		}
	}

	inline bool KeyPress(Keys key) noexcept override
	{
		//auto rows{Rows(table)};

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
		else if (key == KEY_DOWNARROW)
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
		else if (key == KEY_UPARROW)
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
		else if (key == KEY_LEFTARROW)
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
		else if (key == KEY_RIGHTARROW)
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

		return false;
	}

	// Responds to mouse presses
	inline bool MousePress(MouseEvent evt) noexcept override
	{
		return false;
	}

	inline void SetLayout() noexcept override
	{

	}

	inline void SetFocus(bool state) noexcept override
	{

	}

	inline void Destroy() noexcept override
	{
	}

};

class Table : public TableBase<Table>
{

};

} // end namespace cudadoom::txt
