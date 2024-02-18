/**
 * @file windows.h
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

#include <sys/types.h>

#include "windef.h"
#include "winerror.h"
#include "lmerr.h"
#include "winnt.h"
#include "basetsd.h"
#include "winbase.h"

typedef HANDLE HWND;

typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;

typedef struct tagPOINT {
        LONG x;
        LONG y;
} POINT,POINTL,*PPOINT,*LPPOINT,*PPOINTL,*LPPOINTL;

/*
 * winnls.h
 */

#define CP_ACP 0
#define CP_OEMCP 1
#define CP_MACCP 2
#define CP_THREAD_ACP 3
#define CP_SYMBOL 42
#define CP_UTF7 65000
#define CP_UTF8 65001

#define MB_PRECOMPOSED 1
#define MB_COMPOSITE 2
#define MB_ERR_INVALID_CHARS 8
#define MB_USEGLYPHCHARS 4

/*
 * winuser.h
 */

#define WINUSERAPI DECLSPEC_IMPORT

#define WM_COMMAND 273
#define WM_TIMER 275
#define WM_USER 1024

typedef struct tagMSG {
        HWND hwnd;
        UINT message;
        WPARAM wParam;
        LPARAM lParam;
        DWORD time;
        POINT pt;
} MSG,*LPMSG,*PMSG;

/*
 * wincon.h
 */

#define ENABLE_PROCESSED_INPUT		0x0001
#define ENABLE_LINE_INPUT		0x0002
#define ENABLE_ECHO_INPUT		0x0004
#define ENABLE_WINDOW_INPUT		0x0008
#define ENABLE_MOUSE_INPUT		0x0010
#define ENABLE_INSERT_MODE		0x0020
#define ENABLE_QUICK_EDIT_MODE		0x0040
#define ENABLE_EXTENDED_FLAGS  		0x0080
//#define ENABLE_AUTO_POSITION		0x0100

#define ENABLE_PROCESSED_OUTPUT		0x0001
#define ENABLE_WRAP_AT_EOL_OUTPUT	0x0002

typedef struct _COORD {
        SHORT X;
        SHORT Y;
} COORD, *PCOORD;

/*
 * wincrypt.h
 */

typedef ULONG HCRYPTPROV;

WINADVAPI BOOL WINAPI CryptGenRandom(HCRYPTPROV,DWORD,PBYTE);

/*
 * lmaccess.h
 */

#define USER_PRIV_GUEST 0
#define USER_PRIV_USER 1
#define USER_PRIV_ADMIN 2

