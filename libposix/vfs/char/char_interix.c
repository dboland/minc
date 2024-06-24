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

/****************************************************/

BOOL 
AnsiEraseInDisplay(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, CHAR Parm)
{
	DWORD dwScreen;
	DWORD dwCount;
	DWORD dwOffset = 0;
	SMALL_RECT sRect = Info->srWindow;
	COORD cPos = {sRect.Left, sRect.Top};
	SHORT sWidth = sRect.Right - sRect.Left + 1;
	WORD wAttribs = Info->wAttributes & 0xFF;

	if (!Parm || Parm == '0'){
		cPos = Info->dwCursorPosition;
		dwOffset = sRect.Right - cPos.X;
	}else if (Parm == '1'){
		dwOffset = (sRect.Bottom - Info->dwCursorPosition.Y) * sWidth;
		dwOffset += Info->dwCursorPosition.X;
	}else if (Parm == '2'){
		Info->dwCursorPosition = cPos;
		SetConsoleCursorPosition(Handle, cPos);
	}else{
		return(FALSE);
	}
	dwScreen = ((sRect.Bottom - sRect.Top) + 1) * sWidth;
	FillConsoleOutputAttribute(Handle, wAttribs, dwScreen - dwOffset, cPos, &dwCount);
	return(FillConsoleOutputCharacter(Handle, ' ', dwScreen - dwOffset, cPos, &dwCount));
}
BOOL 
AnsiVerticalEditingMode(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, SHORT Parm)
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
BOOL 
AnsiEqualRect(SMALL_RECT *Rect1, SMALL_RECT *Rect2)
{
	DWORDLONG dwlRect1 = *(DWORDLONG *)Rect1;
	DWORDLONG dwlRect2 = *(DWORDLONG *)Rect2;

	if (dwlRect1 != dwlRect2){
		return(FALSE);
	}
	return(TRUE);
}
BOOL 
AnsiSetRect(HANDLE Handle, WORD Top, WORD Bottom, SMALL_RECT *Result)
{
	BOOL bResult = FALSE;
	CONSOLE_SCREEN_BUFFER_INFO csbInfo;

	if (!GetConsoleScreenBufferInfo(Handle, &csbInfo)){
		WIN_ERR("GetConsoleScreenBufferInfo(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else{
		*Result = csbInfo.srWindow;
		Result->Bottom = Result->Top + Bottom - 1;
		Result->Top += Top - 1;
		bResult = !AnsiEqualRect(Result, &csbInfo.srWindow);
	}
	return(bResult);
}
BOOL 
AnsiSetTopBottomMargin(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD Top, WORD Bottom)
{
	SMALL_RECT sRect = Info->srWindow;
	COORD cPos;

	if (AnsiSetRect(Handle, Top, Bottom, &sRect)){
		AnsiVerticalEditingMode(Handle, Info, 1);
	}else{
		AnsiVerticalEditingMode(Handle, Info, 2);
	}
	cPos.X = sRect.Left;
	cPos.Y = sRect.Top;
	Info->srWindow = sRect;
	Info->dwCursorPosition = cPos;
	return(SetConsoleCursorPosition(Handle, cPos));
}
