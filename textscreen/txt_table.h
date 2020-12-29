/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

#include "txt_defines.h"

#include "txt_widget.h"

namespace cudadoom::txt
{
/**
 * Table widget.
 *
 * A table is a widget that contains other widgets. It may have
 * multiple columns, in which case the child widgets are laid out
 * in a grid. Columns automatically grow as necessary, although
 * minimum column widths can be set using @ref TXT_SetColumnWidths.
 *
 * To create a new table, use @ref TXT_NewTable. It is also
 * possible to use @ref TXT_NewHorizBox to create a table, specifying
 * widgets to place inside a horizontal list. A vertical list is
 * possible simply by creating a table containing a single column.
 */
struct txt_table_t
{
	Widget widget;

	// Widgets in this table
	// The widget at (x,y) in the table is widgets[columns * y + x]
	Widget** widgets;
	int num_widgets;

	// Number of columns
	int columns;

	// Currently selected:
	int selected_x;
	int selected_y;
};

extern WidgetClass txt_table_class;
extern Widget txt_table_overflow_right;
extern Widget txt_table_overflow_down;
extern Widget txt_table_eol;
extern Widget txt_table_empty;

void TXT_InitTable(txt_table_t* table, int columns);

/**
 * Create a new table.
 *
 * @param columns		The number of columns in the new table.
 * @return				Pointer to the new table structure.
 */
txt_table_t* TXT_NewTable(int columns);

/**
 * Create a new table and populate it with provided widgets.
 *
 * The arguments to this function are variable. Each argument must be a
 * pointer to a widget, and the list is terminated with a NULL.
 *
 * @param columns		The number of columns in the new table.
 * @return				Pointer to the new table structure.
 */
txt_table_t* TXT_MakeTable(int columns, ...);

/**
 * Create a table containing the specified widgets packed horizontally,
 * from left to right.
 *
 * The arguments to this function are variable. Each argument must
 * be a pointer to a widget, and the list is terminated with a
 * NULL.
 *
 * @return				Pointer to the new table structure.
 */
txt_table_t* TXT_NewHorizBox(TXT_UNCAST_ARG(first_widget), ...);

/**
 * Get the currently selected widget within a table.
 *
 * This function will recurse through subtables if necessary.
 *
 * @param table		The table.
 * @return				Pointer to the widget that is currently selected.
 */
Widget* TXT_GetSelectedWidget(TXT_UNCAST_ARG(table));

/**
 * Add a widget to a table.
 *
 * Widgets are added to tables horizontally, from left to right.
 * For example, for a table with three columns, the first call
 * to this function will add a widget to the first column, the second
 * call to the second column, the third call to the third column,
 * and the fourth will return to the first column, starting a new
 * row.
 *
 * For adding many widgets, it may be easier to use
 * @ref TXT_AddWidgets.
 *
 * @param table		The table.
 * @param widget		The widget to add.
 */
void TXT_AddWidget(TXT_UNCAST_ARG(table), TXT_UNCAST_ARG(widget));

/**
 * Add multiple widgets to a table.
 *
 * Widgets are added as described in the documentation for the
 * @ref TXT_AddWidget function. This function adds multiple
 * widgets. The number of arguments is variable, and the argument
 * list must be terminated by a NULL pointer.
 *
 * @param table		The table.
 */
void TXT_AddWidgets(TXT_UNCAST_ARG(table), ...);

/**
 * Select the given widget that is contained within the specified
 * table.
 *
 * This function will recursively search through subtables if
 * necessary.
 *
 * @param table		The table.
 * @param widget		The widget to select.
 * @return			Non-zero (true) if it has been selected,
 *					or zero (false) if it was not found within
 *					this table.
 */
int TXT_SelectWidget(TXT_UNCAST_ARG(table), TXT_UNCAST_ARG(widget));

/**
 * Change the number of columns in the table.
 *
 * Existing widgets in the table will be preserved, unless the change
 * reduces the number of columns, in which case the widgets from the
 * 'deleted' columns will be freed.
 *
 * This function can be useful for changing the number of columns in
 * a window, which by default are tables containing a single column.
 *
 * @param table			The table.
 * @param new_columns	The new number of columns.
 */
void TXT_SetTableColumns(TXT_UNCAST_ARG(table), int new_columns);

/**
 * Set the widths of the columns of the table.
 *
 * The arguments to this function are variable, and correspond
 * to the number of columns in the table. For example, if a table
 * has five columns, the width of each of the five columns must be
 * specified.
 *
 * The width values are in number of characters.
 *
 * Note that this function only sets the minimum widths for columns;
 * if the columns contain widgets that are wider than the widths
 * specified, they will be larger.
 *
 * @param table		The table.
 */
void TXT_SetColumnWidths(TXT_UNCAST_ARG(table), ...);

/**
 * Remove all widgets from a table.
 *
 * @param table	The table.
 */
void TXT_ClearTable(TXT_UNCAST_ARG(table));

/**
 * Hack to move the selection in a table by a 'page', triggered by the
 * scrollpane. This acts as per the keyboard events for the arrows, but moves
 * the selection by at least the specified number of characters.
 *
 * @param table	The table.
 * @param pagex	Minimum distance to move the selection horizontally.
 * @param pagey	Minimum distance to move the selection vertically.
 * @return			Non-zero if the selection has been changed.
 */
int TXT_PageTable(TXT_UNCAST_ARG(table), int pagex, int pagey);

} /* END NAMESPACE cudadoom::txt */
