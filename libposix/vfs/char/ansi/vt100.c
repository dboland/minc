/*
 * Copyright (c) 2016 Daniel Boland <dboland@xs4all.nl>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of its 
 *    contributors may be used to endorse or promote products derived 
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS 
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <wincon.h>

/****************************************************/

BOOL 
DECSetBottomMargin(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD Top, WORD Bottom)
{
	WORD wHeight;
	COORD cPos = {Info->srWindow.Left, Info->srWindow.Top};

	wHeight = Info->srWindow.Bottom - Info->srWindow.Top;
//	__CTTY->Margin.Top = Top - 1;
//	__CTTY->Margin.Bottom = wHeight - (Bottom - 1);
	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
DECSetMode(HANDLE Handle, WIN_TTY *Terminal, WORD Arg)
{
	BOOL bResult = TRUE;
	CONSOLE_CURSOR_INFO ciNorm = {100, TRUE};

	if (Arg == 4){			// DECSCLM scroll mode, smooth
		Terminal->ScrollRate = 1;
//	}else if (Arg == 6){		// DECOM origin mode, line 1 is relative to margin
	}else if (Arg == 7){		// DECAWM autowrap
		Terminal->Mode.Output |= ENABLE_WRAP_AT_EOL_OUTPUT;
	}else if (Arg == 25){		// show cursor
		bResult = SetConsoleCursorInfo(Handle, &ciNorm);
//	}else if (Arg == 47){		// xterm alternate screen
//	}else if (Arg == 1000){		// linux mouse on
	/* weird: why would a terminal do autoindent? */
	}else if (Arg == 2004){		// xterm bracketed paste mode (nano.exe)
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
DECResetMode(HANDLE Handle, WIN_TTY *Terminal, WORD Arg)
{
	BOOL bResult = TRUE;
	CONSOLE_CURSOR_INFO ciHide = {100, FALSE};
	CONSOLE_CURSOR_INFO ciNorm = {100, TRUE};

	if (Arg == 4){			// DECSCLM scroll mode, jump
		Terminal->ScrollRate = 6;
//	}else if (Arg == 1){		// DECCKM ANSI cursor, needs DECKPAM (ESC =)
	}else if (Arg == 7){		// DECAWM autowrap mode
		Terminal->Mode.Output &= ~ENABLE_WRAP_AT_EOL_OUTPUT;
	}else if (Arg == 12){		// xterm cnorm/cursor_normal
		bResult = SetConsoleCursorInfo(Handle, &ciNorm);
	}else if (Arg == 25){		// hide cursor
		bResult = SetConsoleCursorInfo(Handle, &ciHide);
//	}else if (Arg == 1000){		// mouse off (linux)
//	}else if (Arg == 1049){		// xterm rmcup/exit_ca_mode
	}else if (Arg == 2004){		// xterm bracketed paste mode (nano.exe)
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
DECSelectCharacterSet(HANDLE Handle, CHAR Char)
{
	BOOL bResult = TRUE;

	if (Char == 'A'){	/* UKASCII pound sign */
		bResult = SetConsoleOutputCP(850);
	}else if (Char == 'B'){		/* USASCII dollar sign (bitchx.exe ["Latin-1"]) */
		bResult = SetConsoleOutputCP(437);
	}else if (Char == 'K'){		/* ISO German on LA100,LA120 (bitchx.exe ["user-defined"]) */
		bResult = SetConsoleOutputCP(GetOEMCP());
	}else if (Char == 'U'){		/* IBM 437 (bitchx.exe ["dumb mode"]) */
		bResult = SetConsoleOutputCP(437);
	}else if (Char == '0'){		/* DEC VT100 line drawing set (ACS) */
		bResult = SetConsoleOutputCP(437);
	}else if (Char == '1'){		/* Alternate character ROM set */
		bResult = SetConsoleOutputCP(GetACP());
	}else if (Char == '2'){		/* Alternate character ROM set with line drawing */
		bResult = SetConsoleOutputCP(GetOEMCP());
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
DECResetCharacterSet(HANDLE Handle, CHAR Char)
{
	BOOL bResult = TRUE;

	if (Char == 'A'){
		bResult = SetConsoleOutputCP(CP_UTF8);
	}else if (Char == 'B'){
		bResult = SetConsoleOutputCP(1252);
	}else if (Char == 'K'){
		bResult = SetConsoleOutputCP(GetACP());
	}else if (Char == 'U'){
		bResult = SetConsoleOutputCP(CP_UTF8);
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
DECSaveCursor(CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	COORD cPos = Info->dwCursorPosition;

	__CTTY->Cursor.X = cPos.X;
	__CTTY->Cursor.Y = cPos.Y - Info->srWindow.Top;
	return(TRUE);
}
BOOL 
DECRestoreCursor(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	COORD cPos = __CTTY->Cursor;

	cPos.Y += Info->srWindow.Top;
	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
DECResetInitialState(HANDLE Handle)
{
	return(TRUE);
}
