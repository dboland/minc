/**
 * @file wincon.h
 * Copyright 2012, 2013 MinGW.org project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#define ENABLE_PROCESSED_INPUT		0x0001
#define ENABLE_LINE_INPUT		0x0002
#define ENABLE_ECHO_INPUT		0x0004
#define ENABLE_WINDOW_INPUT		0x0008
#define ENABLE_MOUSE_INPUT		0x0010
#define ENABLE_INSERT_MODE		0x0020
#define ENABLE_QUICK_EDIT_MODE		0x0040
#define ENABLE_EXTENDED_FLAGS  		0x0080

#define ENABLE_PROCESSED_OUTPUT		0x0001
#define ENABLE_WRAP_AT_EOL_OUTPUT	0x0002

typedef struct _COORD {
        SHORT X;
        SHORT Y;
} COORD, *PCOORD;

typedef struct _SMALL_RECT {
	SHORT Left; 
	SHORT Top; 
	SHORT Right; 
	SHORT Bottom; 
} SMALL_RECT; 

typedef struct _CONSOLE_SCREEN_BUFFER_INFO {
	COORD dwSize;
	COORD dwCursorPosition;
	WORD wAttributes;
	SMALL_RECT srWindow;
	COORD dwMaximumWindowSize;
} CONSOLE_SCREEN_BUFFER_INFO;

