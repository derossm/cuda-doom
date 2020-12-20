//
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//

#ifndef SETUP_DISPLAY_H 
#define SETUP_DISPLAY_H

void ConfigDisplay(void *widget, void *user_data);
void SetDisplayDriver(void);
void BindDisplayVariables(void);

extern int show_endoom;
extern int graphical_startup;
extern int png_screenshots;

#endif /* #ifndef SETUP_DISPLAY_H */
