/**
 * @file windef.h
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
#ifndef _WINDEF_H
#define _WINDEF_H

#undef __stdcall				/* MinGW gcc.exe */
#define __stdcall __declspec(stdcall)

#define VOID void
#define CONST const
#define FAR

#define FALSE 0
#define TRUE 1
#define WINAPI __stdcall
#define PASCAL __stdcall
//#define DECLSPEC_IMPORT __declspec(dllimport)
#define DECLSPEC_IMPORT 
#define DECLSPEC_NORETURN __declspec(noreturn)
#define DECLARE_STDCALL_P( type ) __stdcall type
#define CALLBACK __stdcall
#define _ANONYMOUS_UNION	
#define _ANONYMOUS_STRUCT	
#define MAX_PATH 260

typedef uint32_t DWORD,*PDWORD,*LPDWORD;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef int INT, *LPINT;
typedef int BOOL;
typedef unsigned int UINT;

typedef INT (WINAPI *FARPROC)();
typedef BYTE *PBYTE,*LPBYTE;
typedef CONST VOID *PCVOID,*LPCVOID;

#endif
