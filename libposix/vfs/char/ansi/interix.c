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

#define VK_MODIFY	0x1F

/* 0x1F - 0x00 */

#define VK_CURSOR		0x2F
#define ANSI_CURSOR(k)		__ANSI_CURSOR[VK_CURSOR - k]

/* 0x2F - 0x20 */

static LPCSTR __ANSI_CURSOR[] = {
	"",		/* VK_HELP */
	"\177",		/* VK_DELETE: previously \e[M */
	"\e[L",		/* VK_INSERT */
	"",		/* VK_SNAPSHOT */
	"",		/* VK_EXECUTE */
	"",		/* VK_PRINT */
	"",		/* VK_SELECT */
	"\e[B",		/* VK_DOWN */
	"\e[C",		/* VK_RIGHT */
	"\e[A",		/* VK_UP */
	"\e[D",		/* VK_LEFT */
	"\e[H",		/* VK_HOME */
	"\e[F",		/* VK_END */
	"\e[T",		/* VK_NEXT */
	"\e[S",		/* VK_PRIOR */
	""		/* VK_SPACE */
};

#define VK_FUNCTION		0x8F
#define ANSI_FUNCTION(k)	__ANSI_FUNCTION[VK_FUNCTION - k]

/* 0x8F - 0x70 */

static LPCSTR __ANSI_FUNCTION[] = {
	"",		/* Reserved */
	"",		/* Reserved */
	"",		/* Reserved */
	"",		/* Reserved */
	"",		/* Reserved */
	"",		/* Reserved */
	"",		/* Reserved */
	"",		/* Reserved */
	"\eF24",
	"\eF23",
	"\eF22",
	"\eF21",
	"\eF20",
	"\eF19",
	"\eF18",
	"\eF17",
	"\eF16",
	"\eF15",
	"\eF14",
	"\eF13",
	"\eF12",
	"\eF11",
	"\eF10",
	"\eF9",
	"\eF8",
	"\eF7",
	"\eF6",
	"\eF5",
	"\eF4",
	"\eF3",
	"\eF2",
	"\eF1"
};

/* Constants for ScreenAnsi() */

#define ANSI_REP		'_'
#define ANSI_VEM		'b'

/****************************************************/

BOOL 
AnsiEraseInDisplay(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, CHAR Parm)
{
	DWORD dwScreen;
	DWORD dwCount;
	DWORD dwOffset = 0;
	SMALL_RECT sRect = Info->srWindow;
	COORD cPos = {sRect.Left, sRect.Top};

	if (!Parm || Parm == '0'){
		cPos = Info->dwCursorPosition;
		dwOffset = sRect.Right - cPos.X;
	}else if (Parm == '1'){
		dwOffset = (sRect.Bottom - Info->dwCursorPosition.Y) * ((sRect.Right - sRect.Left) + 1);
		dwOffset += Info->dwCursorPosition.X;
	}else if (Parm == '2'){
		SetConsoleCursorPosition(Handle, cPos);
	}else{
		return(FALSE);
	}
	dwScreen = ((sRect.Bottom - sRect.Top) + 1) * ((sRect.Right - sRect.Left) + 1);
	FillConsoleOutputAttribute(Handle, Info->wAttributes, dwScreen - dwOffset, cPos, &dwCount);
	return(FillConsoleOutputCharacter(Handle, ' ', dwScreen - dwOffset, cPos, &dwCount));
}
BOOL 
AnsiRepeat(HANDLE Handle, CHAR C, SHORT Count)
{
	return(TRUE);
}
BOOL 
AnsiVerticalEditingMode(HANDLE Handle, SHORT Parm)
{
	BOOL bResult = TRUE;

	if (Parm == 1){
		__CTTY->VEdit = TRUE;
	}else if (Parm == 2){
		__CTTY->VEdit = FALSE;
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
