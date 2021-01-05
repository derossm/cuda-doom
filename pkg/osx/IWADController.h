/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

// TODO What are these AppKits?
#ifdef APPKIT_EXISTS
#undef APPKIT_EXISTS
#endif

#ifdef APPKIT_EXISTS
#include <AppKit/AppKit.h>
#include <AppKit/NSNibLoading.h>

@interface IWADController : NSObject
{
	id iwadSelector;
	id configWindow;

	id chex;
	id doom1;
	id doom2;
	id plutonia;
	id tnt;
	id freedoom1;
	id freedoom2;
	id freedm;

	id heretic;
	id hexen;
	id strife;
}

- (void) closeConfigWindow: (id)sender;
- (void) openConfigWindow: (id)sender;
- (NSString*) getIWADLocation;
- (NSString*) autoloadPath;
- (void) awakeFromNib;
- (bool) setDropdownList;
- (void) setDropdownSelection;
- (void) saveConfig;
- (std::string) doomWadPath;
- (void) setEnvironment;
- (std::string) getGameName;
- (bool) addIWADPath: (NSString*) path;
- (bool) selectGameByName: (std::string) name;

@end
#endif /* APPKIT_EXISTS */
