# Coding style guidelines

Rule 0: 
Rule 1: Thou shalt never use signed values for lengths, sizes, or index positions.
Rule 2: Thou shalt declare no more than one object in any one statement.
Rule 3: TBD
Rule 4: 
Rule 5: 
Rule 6: 
Rule 7: 
Rule 8: 
Rule 9: 
Rule A: 
Rule B: 
Rule C: 
Rule D: 
Rule E: 
Rule F: 

The coding style guidelines for Cuda Doom are designed to COMPLETELY OVERHAUL EVERYTHING ABOUT the original source code to conform to modern C++20 standards.

Start by following the Core Guidelines. Then profile and optimize for efficiency in the most HOT code; everywhere else, prioritize readibility and maintainability.

Tabs should always be set to display as four spaces, otherwise your identations will look wrong. Use ONLY tabs for indentation; always align to tabs, even if 1-3 spaces here or there might look more aesthetically pleasing. The only exceptions are for ASCII art and creating elaborately decorated comments when you are bored.

All code should always be TAB aligned.

Try writing code to 144 width (consider refactoring with more object wrappers if this is insufficient space to avoid multi-line statements); hard wrap code with
160 width (note: above line is exactly 160 characters, with a line break at column 161, if your editor isn't pre-maturely wrapping).

This is 80 characters:
```cpp
/******************************************************************************/
```

This is 144 characters:
```cpp
/**********************************************************************************************************************************************/
```
This is 160 characters:
```cpp
/**************************************************************************************************************************************************************/
```

Comments may be aligned to any preference, if decorating them as described earlier, but otherwise consider allowing users to define their own wrapping preferences. I know we all have the instinct to use hard breaks, to control with absolute precision how our comments display on the screen, but we probably should move forward with modern UX instead of "hard coding" the line breaks in our documentation when it doesn't directly affect our pretty ASCII art and aligned tables etc.

Do not leave trailing whitespace at the end of lines. Do not mix tabs and spaces in code. Do no use repeating spaces in code.

Do not use double spaces after periods in your comments; that convention is from the days of typewriters and has no business on computers.
See https://www.instructionalsolutions.com/blog/one-space-vs-two-after-period for why using two spaces is wrong.

Global functions should be named like this: `AB_FunctionName`. The `AB` prefix denotes the subsystem (`AM_` for automap, `G_` for game, etc). Global functions and global variables should always be made static if possible, but should also both be avoided like the plague.

Almost never use typedef, but if you find some genuine use case, put `_t` on the end of types created with typedef. Type names like this should be all lowercase and have the subsystem name at the start. An example of this is `txt_window_t`.

If you are considering using GOTO, you are probably making a mistake and should re-think your entire design and the algorithms you are using to implement it. If GOTO still seems appropriate, profile the code with and without using GOTO, and if the difference is within 1%, don't use GOTO unless the code is EXTREMELY HOT. If the difference is a full percentage or greater (and the code section is EXTREMELY HOT), document these results, and add a note to reprofile this code every time a major update to the compiler happens as well as advising anyone bored to to consider ways to refactor this code without using GOTO while not sacrificing performance.

Use `//` for short comments; use `\* *\` for detailed comments describing complicated behavior, brief comments nested inside code statements, and heading comment blocks.

Types should be named `MyObjectName` for object declaration and definitions, object instances should be named `myObjectName`; public methods should use the `myMemberMethod` naming style; private members can use `_myMemberName` to distinguish internal usage, where there needs to be a separate public member with the same logical name. Global constants should use `MY_GLOBAL_CONSTANT` style, and global functions should use `my_global_function` style.

There shouldn't be any global variables, so there is no style guideline for how to name them. Consider `REPLACE_THIS_GLOBAL_MY_GLOBAL` so you don't forget to fix this hack later, if you must use a global variable.

In pointer variable declarations, place `*` next to the Type name, not the variable name; the pointer-ness is part of the object Type, not part of the object identifier.

If the type is a constant pointer, then use the `Type *const`. This is not a pointer, so it isn't a `*` by it-self; this is a const-pointer, which should be thought of as a different thing than a pointer and having a different representation; the entire `*const` is the const-pointer, and the two symbols are conceptually inseparable.

Note that `const Type*` is a pointer to a `const Type` where the two words together describe one thing at which to be pointed; conceptually this
can be thought of as `(const Type)*`, even if syntactically the parens aren't allowed. 

When using an if, do, while, or for statement, always use the { } braces even when they are not necessary. For example, do this:

```cpp
if (condition)
{
	body;
}
```

Not this:

```cpp
if (condition)	// NO
	body;
```

{NOTE: do not use this many arguments, make a new type to pass if you must pass this much information}
{NOTE: use meaningful variable names, and meaningful zero-cost type wrappers instead of using int for everything}
```cpp
void FunctionName(int argument, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7)
{
	auto assign_var{arg2 + arg3 * arg4 * (arg5 + arg6)};

	if (foo && !bar || baz && qux || !(foo && bar && baz))
	{
		body;
	}
	else if (xyz + 4 < abc * 4 + 3)
	{
		body;
	}
	else
	{
		body;
	}

	if (veryLongConditionLikeThisOneThatDoeNotForceLineBreak && other_condition)
	{
		body;
	}

	// NOTE: switches are actually GOTO's with window dressing, so use them sparringly
	switch (argument)
	{
		case FIRST:
			code;
			break;

		case SECOND:
			code;
			break;

		default:
			break;
	}

	// NOTE: avoid raw for loops
	for (auto i{0ull}; i < 10; ++i)
	{
		FunctionCall(arg1, arg2, arg3, arg4, argNotSplitOntoSecondLine);
	}

	auto k{0ull};
	while (k < 10)
	{
		loop_body;
	}

	do
	{
		loop_body;
	} while (condition);
}
```

## Portability

CUDA Doom is designed to be somewhat cross-platform, but also require a modern OS (Windows10, Ubuntu, Android), modern hardware (AVX2), and focus on nvidia optimizations with CUDA. There is no support for OSX, iPhone, etc. Possible support for Xbox One, Xbox Series X/S, PS4, PS5, PS Vita, Nintendo Switch, etc.

Use Doom’s byte data type for byte data. Prefer auto for most types, but specify `size_t` for sizes, lengths, and indicies etc.

You can assume little-endianess; no system we care about in use today still uses big-endian.

For signed integers, you shouldn’t assume that `(i >> n)` is the same as `(i / (1 << n))`. However, you shouldn't be using bitshifts, so this isn't important to think about.

## GNU GPL and licensing

All code submitted to the project must be licensed under the GNU GPLv2 or a compatible license. If you use code that you haven’t 100% written yourself, say so. Add a copyright header to the start of every file. Use this template:

```cpp
/**********************************************************************************************************************************************\
	Copyright(C) YEAR Author's Name

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	<File description goes here>
\**********************************************************************************************************************************************/
#pragma once
```
