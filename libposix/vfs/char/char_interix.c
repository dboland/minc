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
#define ANSI_CURSOR(vk)		__ANSI_CURSOR[VK_CURSOR - vk]

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

#define VK_WINDOWS	0x5F

/* 0x5F - 0x50 */

#define VK_FUNCTION		0x8F
#define ANSI_FUNCTION(vk)	__ANSI_FUNCTION[VK_FUNCTION - vk]

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
	"\eFO",		/* F24 */
	"\eFN",		/* F23 */
	"\eFM",		/* F22 */
	"\eFL",		/* F21 */
	"\eFK",		/* F20 */
	"\eFJ",		/* F19 */
	"\eFI",		/* F18 */
	"\eFH",		/* F17 */
	"\eFG",		/* F16 */
	"\eFF",		/* F15 */
	"\eFE",		/* F14 */
	"\eFD",		/* F13 */
	"\eFC",		/* F12 */
	"\eFB",		/* F11 */
	"\eFA",		/* F10 */
	"\eF9",		/* F9 */
	"\eF8",		/* F8 */
	"\eF7",		/* F7 */
	"\eF6",		/* F6 */
	"\eF5",		/* F5 */
	"\eF4",		/* F4 */
	"\eF3",		/* F3 */
	"\eF2",		/* F2 */
	"\eF1"		/* F1 */
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
AnsiVerticalEditingMode(HANDLE Handle, SMALL_RECT *Rect, SHORT Parm)
{
	BOOL bResult = TRUE;

	if (Parm == 1){
		__CTTY->Margin = *Rect;
		__CTTY->VEdit = TRUE;
	}else if (Parm == 2){
		__CTTY->VEdit = FALSE;
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
AnsiEqualRect(SMALL_RECT *Rect1, SMALL_RECT *Rect2)
{
	DWORD dwSum1 = Rect1->Left + Rect1->Right + Rect1->Top + Rect1->Bottom;
	DWORD dwSum2 = Rect2->Left + Rect2->Right + Rect2->Top + Rect2->Bottom;

	if (dwSum1 != dwSum2){
		return(FALSE);
	}
	return(TRUE);
}
BOOL 
AnsiSetTopBottomMargin(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD Top, WORD Bottom)
{
	SMALL_RECT sRect = Info->srWindow;
	COORD cPos = {sRect.Left, sRect.Top + (Top - 1)};

	/* Margins will not work without disabling the 'am' capability
	 * (ENABLE_WRAP_AT_EOL_OUTPUT), so long lines can be scrolled
	 * as one.
	 */
	sRect.Bottom = sRect.Top + Bottom - 1;
	sRect.Top += Top - 1;
	if (AnsiEqualRect(&Info->srWindow, &sRect)){
		AnsiVerticalEditingMode(Handle, &sRect, 2);
	}else{
		AnsiVerticalEditingMode(Handle, &sRect, 1);
	}
	return(SetConsoleCursorPosition(Handle, cPos));
}
