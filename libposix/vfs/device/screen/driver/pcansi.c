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

#define ANSI_PRIOR	"\e[5~"
#define ANSI_NEXT		"\e[6~"
#define ANSI_END		"\e[4~"
#define ANSI_HOME		"\e[H"
#define ANSI_LEFT		"\e[D"
#define ANSI_UP		"\e[A"
#define ANSI_RIGHT	"\e[C"
#define ANSI_DOWN		"\e[B"
#define ANSI_INSERT	"\e[L"
#define ANSI_DELETE	"\e[3~"
#define ANSI_F1		"\e[11~"
#define ANSI_F2		"\e[12~"
#define ANSI_F3		"\e[13~"
#define ANSI_F4		"\e[14~"
#define ANSI_F5		"\e[15~"
#define ANSI_F6		"\e[16~"
#define ANSI_F7		"\e[17~"
#define ANSI_F8		"\e[18~"
#define ANSI_F9		"\e[19~"
#define ANSI_F10		"\e[20~"
#define ANSI_F11		"\e[21~"
#define ANSI_F12		"\e[22~"

//#define ANSI_LEFT		"\e\316K"	/* real pcansi (vim.exe) */

#define ANSI_REP		'b'
#define ANSI_VEM		'_'

#define FOREGROUND_DEFAULT		(FOREGROUND_WHITE)
#define BACKGROUND_DEFAULT		(BACKGROUND_BLACK)

#define FOREGROUND_UNDERLINE		(FOREGROUND_CYAN)

/****************************************************/

BOOL 
AnsiEraseInDisplay(HANDLE Screen, CSB_INFO *Info, CHAR Char)
{
	DWORD dwScreen;
	DWORD dwCount;
	DWORD dwOffset = 0;
	SMALL_RECT sRect = Info->srWindow;
	COORD cPos = {sRect.Left, sRect.Top};

	if (!Char || Char == '0'){
		cPos = Info->dwCursorPosition;
		dwOffset = sRect.Right - cPos.X;
	}else if (Char == '1'){
		dwOffset = (sRect.Bottom - Info->dwCursorPosition.Y) * ((sRect.Right - sRect.Left) + 1);
		dwOffset += Info->dwCursorPosition.X;
	}else if (Char != '2'){
		return(FALSE);
	}
	dwScreen = ((sRect.Bottom - sRect.Top) + 1) * ((sRect.Right - sRect.Left) + 1);
	FillConsoleOutputAttribute(Screen, Info->wAttributes, dwScreen - dwOffset, cPos, &dwCount);
	return(FillConsoleOutputCharacter(Screen, ' ', dwScreen - dwOffset, cPos, &dwCount));
}
BOOL 
AnsiRepeat(HANDLE Screen, CHAR C, UINT Count)
{
	DWORD dwCount;

	while (Count--){
		WriteConsole(Screen, &C, 1, &dwCount, NULL);
	}
	return(TRUE);
}
BOOL 
AnsiVerticalEditingMode(HANDLE Screen, SHORT Parm)
{
	return(TRUE);
}
