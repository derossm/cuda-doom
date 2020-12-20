# Coding style guidelines

The coding style guidelines for Chocolate Doom are designed to keep the style of the original source code. This maintains consistency throughout
the program, and does not require the original code to be changed. Some of these guidelines are stricter than what was done in the original
source; follow these when writing new code only: there is no need to change existing code to fit them.

You should set tabs to *display* as eight spaces, not four.
{STRONGLY DISAGREE: Set TAB to display as 4 character spaces}

However, *indentation* should be four spaces. If possible, do not use tab characters at all.
{STRONGLY DISAGREE: ONE character vs FOUR characters is a free compression ratio of 4:1; ALWAYS use tabs for indentation}

There is a utility called “expand” which will remove tab characters. For the {INCORRECT} reasoning behind this, see:
http://www.jwz.org/doc/tabs-vs-spaces.html

Please write code to an 80 column limit so that it fits within a standard 80 column terminal.
{STRONGLY DISAGREE: use at least 144 width; this is not the 1980's, and no one has smaller than 1920 pixel width displays. Also, consider allowing users to define their own wrapping preferences. I still have the old itch to use hard breaks myself, but we probably should move forward with modern UX instead of "hard coding" the line breaks in our documentation when it doesn't directly affect our pretty ASCII art and aligned tables etc.}

Do not leave trailing whitespace at the end of lines.

Functions should be named like this: `AB_FunctionName`. The `AB` prefix denotes the subsystem (`AM_` for automap, `G_` for game, etc). If a
function is static, you can omit the prefix and just name it like `FunctionName`. {Global} Functions and global variables should always be made
static if possible.

Put `_t` on the end of types created with typedef.
{NOTE: do not use typedefs, but if you must, do follow this convention of postfixing `_t` to the typename}

Type names like this should be all lowercase and have the subsystem name at the start. An example of this is `txt_window_t`. When creating
structures, always typedef them.
{STRONGLY DISAGREE: NEVER use typedef when defining object interfaces}

Do not use Hungarian notation.
{NOTE: use the notation described here, whether or not it falls into Hungarian or any other notation style}

Do not use the GOTO statement.
{NOTE: unless you have a really good reason, such as DEMONSTRABLE profiled performance increase in extremely hot sections of code and
known alternatives fail to approach performance parity}

Use C++-style comments, ie. `//` comments, not `/* ... */` comments. I don’t care that this isn’t standard ANSI C.
{STRONGLY DISAGREE: use `/* ... */` for multi-line comments and `//` for single-line comments}

Variables should be named like this: `my_variable_name`, not like this: `MyVariableName`.
{STRONGLY DISAGREE: `my_variable_name` for global scope functions only (which should be extremely rare),
use `MyVariableName` for object declaration and definitions, and use `myVariableName` for object instances.}

In pointer variable declarations, place the `*` next to the variable name, not the type.
{STRONGLY DISAGREE: Pointer is part of the Type}

When casting variables from one type to another, put a space after the last closing brace.
{NOTE: don't cast unless absolutely necessary for performance optimization}

When using an if, do, while, or for statement, always use the { } braces even when they are not necessary. For example, do this:

```c
if (condition)
{
	body;
}
```

Not this:

```c
if (condition)	// NO
	body;
```

Write code like this:

```c
typedef struct
{
	int member1;
	char* member2;
} my_structure_t;
{NOTE: irrelevant since we aren't using typedef struct nonsense}

{NOTE: do not use this many arguments, make a new type to pass if you must pass this much information}
{NOTE: use meaningful variable names, and meaningful zero-cost type wrappers instead of using int for everything}
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

	{NOTE: switches are actually GOTO's with window dressing, so use them sparringly}
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

	{NOTE: avoid raw for loops}
	for (auto a{0}; a < 10; ++a)
	{
		FunctionCall(arg1, arg2, arg3, arg4, argNotSplitOntoSecondLine);
	}

	auto a{0};
	while (a < 10)
	{
		loop_body;
	}

	do
	{
		loop_body;
	} while (condition);
}
```

## Editor-specific default settings

If you use vim, you can put this into your `.vimrc` (or install the
`localvimrc` script):

```
set tabstop=4
set softtabstop=4
set shiftwidth=4
```

Or, if you use Emacs, put this in your `.emacs`:

```lisp
(add-hook 'c-mode-hook (lambda ()
(when (and buffer-file-name
		(string-match "chocolate-doom" buffer-file-name))
(c-set-style "bsd")
(setq tab-width 4)
(setq c-basic-offset 4))))
```

## Security

The C standard library has a number of unsafe functions that should be avoided when writing code for Chocolate Doom. These are:

Unsafe function	|	Safer alternative
----------------|------------------------
`gets()`		|	`fgets(.., stdin)`
`sprintf`		|	`M_snprintf()`
`snprintf`		|	`M_snprintf()`
`vsprintf`		|	`M_vsnprintf()`
`vsnprintf`		|	`M_vsnprintf()`
`strcpy()`		|	`M_StringCopy()`
`strncpy()`		|	`M_StringCopy()`
`strcat()`		|	`M_StringConcat()`
`strncat()`		|	`M_StringConcat()`
`strdup()`		|	`M_StringDuplicate()`
`realloc()`		|	`I_Realloc()`
{NOTE: avoid ALL of these old C functions and design for modern algorithms}

Lots of the code includes calls to DEH_String() to simulate string replacement by the Dehacked tool. Be careful when using Dehacked
replacements of printf format strings. For example, do not do this:

```c
printf(DEH_String("foo %s"), s);
sprintf(mybuf, DEH_String("bar %s"), t);
```

Instead do this:

```c
DEH_printf("foo %s", s);
DEH_snprintf(mybuf, sizeof(mybuf), "bar %s", t);
```

This does the format string replacement safely in a way that checks the arguments securely.
{STRONGLY DISAGREE: instead, let's not use any hacked code at all and design for modern CPU's and GPU's with C++20 and CUDA}

## Portability

Chocolate Doom is designed to be cross-platform and work on different Operating Systems and processors. Bear this in mind when writing code.

Do not use the `long` type (its size differs across platforms; use `int` or `int64_t` depending on which you want).

Use Doom’s byte data type for byte data. `int` is assumed to be a 32-bit integer, and `short` is a 16-bit integer. You can also use the
ISO C99 data types: `intN_t` and `uintN_t` where N is 8, 16, 32, 64.
{SOMEWHAT DISAGREE: use C++ type system to the fullest, but do avoid use of int, short, long when it matters; prefer auto when it doesn't}

Be careful with platform dependencies: do not use Windows API functions, for example. Use SDL where possible.

Preprocessor `#defines` are set that can be used to identify the OS if necessary: `_WIN32` for Windows and `__MACOSX__` for Mac OS X. Others
are set through SDL. Try to avoid this if possible.

Be careful of endianness! Doom has `SHORT()` and `LONG()` macros that do endianness conversion. Never assume that integer types have a
particular byte ordering. Similarly, never assume that fields inside a structure are aligned in a particular way. This is most
relevant when reading or writing data to a file or a network pipe.
{SOMEWHAT DISAGREE: little endian WON the war a long time ago, but your code shouldn't rely on byte ordering anyway}

For signed integers, you shouldn’t assume that `(i >> n)` is the same as `(i / (1 << n))`. However, most processors handle bitshifts of signed
integers properly, so it’s not a huge problem.
{STRONGLY DISAGREE: avoid using SHIFT operators; only consider for final stage performance optimizations where profiled bottlenecks exist}

## GNU GPL and licensing

All code submitted to the project must be licensed under the GNU GPLv2 or a
compatible license. If you use code that you haven’t 100% written
yourself, say so. Add a copyright header to the start of every file. Use
this template:

```
//
// Copyright(C) YEAR Author's name
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
//
// *File description goes here*
//
```
