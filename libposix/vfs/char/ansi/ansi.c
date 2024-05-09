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

#define FOREGROUND_WHITE	(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define FOREGROUND_YELLOW	(FOREGROUND_RED | FOREGROUND_GREEN)
#define FOREGROUND_MAGENTA	(FOREGROUND_BLUE | FOREGROUND_RED)
#define FOREGROUND_CYAN		(FOREGROUND_BLUE | FOREGROUND_GREEN)
#define FOREGROUND_BLACK	(0x0)
#define FOREGROUND_DEFAULT	(FOREGROUND_WHITE)
#define FOREGROUND_UNDERLINE	(FOREGROUND_CYAN)
#define FOREGROUND_RGB		(FOREGROUND_YELLOW)

#define BACKGROUND_WHITE	(BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE)
#define BACKGROUND_YELLOW	(BACKGROUND_RED | BACKGROUND_GREEN)
#define BACKGROUND_MAGENTA	(BACKGROUND_BLUE | BACKGROUND_RED)
#define BACKGROUND_CYAN		(BACKGROUND_BLUE | BACKGROUND_GREEN)
#define BACKGROUND_BLACK	(0x0)
#define BACKGROUND_DEFAULT	(BACKGROUND_BLACK)
#define BACKGROUND_RGB		(BACKGROUND_YELLOW)

/****************************************************/

LONG 
AnsiStrToInt(LPCSTR String)
{
	LONG lResult = 0;

	if (!(lResult = atoi(String))){
		lResult = 1;
	}
	return(lResult);
}
BOOL 
AnsiEraseRect(HANDLE Handle, SMALL_RECT Rect, WORD Attributes)
{
	COORD cPos = {Rect.Left, Rect.Top};
	DWORD dwCount = ((Rect.Bottom - Rect.Top) + 1) * ((Rect.Right - Rect.Left) + 1);

	FillConsoleOutputAttribute(Handle, Attributes, dwCount, cPos, &dwCount);
	return(FillConsoleOutputCharacter(Handle, ' ', dwCount, cPos, &dwCount));
}
BOOL 
AnsiScrollHorizontal(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD DeltaLeft, WORD DeltaRight)
{
	DWORD dwCount;
	SMALL_RECT sRect;
	CHAR_INFO cInfo;
	COORD cPos = Info->dwCursorPosition;

	cPos.X += DeltaRight;
	sRect.Left = Info->dwCursorPosition.X + DeltaLeft;
	sRect.Right = Info->dwSize.X - DeltaRight;
	sRect.Top = Info->dwCursorPosition.Y;
	sRect.Bottom = Info->dwCursorPosition.Y;
	cInfo.Char.AsciiChar = ' ';
	cInfo.Attributes = Info->wAttributes;
	return(ScrollConsoleScreenBuffer(Handle, &sRect, NULL, cPos, &cInfo));
}
WORD 
AnsiInvertAttrib(CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	WORD wBackGround = (Info->wAttributes & 0xF0) / 0x10;
	WORD wForeGround = (Info->wAttributes & 0x0F) * 0x10;

	return(wBackGround | wForeGround);
}
BOOL 
AnsiCursorMove(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD DeltaY, WORD DeltaX)
{
	COORD cPos = Info->dwCursorPosition;

	cPos.Y += DeltaY;
	cPos.X += DeltaX;
	return(SetConsoleCursorPosition(Handle, cPos));
}

/****************************************************/

BOOL 
AnsiRenderForeground(CONSOLE_SCREEN_BUFFER_INFO *Info, CHAR Char2)
{
	BOOL bResult = TRUE;
	WORD wAttrib = Info->wAttributes & 0xF8;

	if (Char2 == '0'){
		wAttrib |= FOREGROUND_BLACK;
	}else if (Char2 == '1'){
		wAttrib |= FOREGROUND_RED;
	}else if (Char2 == '2'){
		wAttrib |= FOREGROUND_GREEN;
	}else if (Char2 == '3'){
		wAttrib |= FOREGROUND_YELLOW;
	}else if (Char2 == '4'){
		wAttrib |= FOREGROUND_BLUE;
	}else if (Char2 == '5'){
		wAttrib |= FOREGROUND_MAGENTA;
	}else if (Char2 == '6'){
		wAttrib |= FOREGROUND_CYAN;
	}else if (Char2 == '7'){
		wAttrib |= FOREGROUND_WHITE;
//	}else if (Char2 == '8'){ 	/* RGB color follows (journalctl in Debian) */
	}else if (Char2 == '9'){	/* default (ANSI v.2.53) */
		wAttrib |= FOREGROUND_DEFAULT;
	}else{
		bResult = FALSE;
	}
	Info->wAttributes = wAttrib;
	return(bResult);
}
BOOL 
AnsiRenderBackground(CONSOLE_SCREEN_BUFFER_INFO *Info, CHAR Char2)
{
	BOOL bResult = TRUE;
	WORD wAttrib = Info->wAttributes & 0x8F;

	if (Char2 == '0'){
		wAttrib |= BACKGROUND_BLACK;
	}else if (Char2 == '1'){
		wAttrib |= BACKGROUND_RED;
	}else if (Char2 == '2'){
		wAttrib |= BACKGROUND_GREEN;
	}else if (Char2 == '3'){
		wAttrib |= BACKGROUND_YELLOW;
	}else if (Char2 == '4'){
		wAttrib |= BACKGROUND_BLUE;
	}else if (Char2 == '5'){
		wAttrib |= BACKGROUND_MAGENTA;
	}else if (Char2 == '6'){
		wAttrib |= BACKGROUND_CYAN;
	}else if (Char2 == '7'){
		wAttrib |= BACKGROUND_WHITE;
//	}else if (Char2 == '8'){ 	/* RGB color follows */
	}else if (Char2 == '9'){	/* default (ANSI v.2.53) */
		wAttrib |= BACKGROUND_DEFAULT;
	}else{
		bResult = FALSE;
	}
	Info->wAttributes = wAttrib;
	return(bResult);
}
BOOL 
AnsiRenderFont(CONSOLE_SCREEN_BUFFER_INFO *Info, CHAR Char2)
{
	BOOL bResult = TRUE;
	WORD wIntensity = Info->wAttributes & BACKGROUND_INTENSITY;

	if (!Char2 || Char2 == '0'){		/* Clear all attributes */
		Info->wAttributes = BACKGROUND_DEFAULT | FOREGROUND_DEFAULT;
		__CTTY->RVideo = FALSE;		/* GNU less.exe */
	}else if (Char2 == '1'){		/* bold */
		Info->wAttributes |= FOREGROUND_INTENSITY;
	}else if (Char2 == '2'){		/* dim or secondary (journalctl on Debian) */
		bResult = TRUE;
	}else if (Char2 == '3'){		/* italic (journalctl on Debian) */
		bResult = TRUE;
	}else if (Char2 == '4'){		/* underscore (man.exe) */
		Info->wAttributes |= FOREGROUND_UNDERLINE;
	}else if (Char2 == '5'){		/* slow blink (vim.exe) */
		__CTTY->RVideo = TRUE;
//	}else if (Char2 == '6'){		/* fast blink */
	}else if (Char2 == '7'){		/* invert (smso: enter standout mode) */
		__CTTY->RVideo = TRUE;
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
AnsiRenderClear(CONSOLE_SCREEN_BUFFER_INFO *Info, CHAR Char2)
{
	BOOL bResult = TRUE;

	if (Char2 == '2'){		/* bold */
		Info->wAttributes &= ~FOREGROUND_INTENSITY;
	}else if (Char2 == '4'){	/* underline */
		Info->wAttributes &= ~FOREGROUND_UNDERLINE;
	}else if (Char2 == '5'){	/* slow blink (bitchx.exe "dumb" mode) */
		__CTTY->RVideo = FALSE;
	}else if (Char2 == '7'){	/* inversion */
		__CTTY->RVideo = FALSE;
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
AnsiRender(CONSOLE_SCREEN_BUFFER_INFO *Info, CHAR Char1, CHAR Char2, CHAR Char3)
{
	BOOL bResult = FALSE;

	if (Char1 == '0'){			/* raspbian login */
		bResult = AnsiRenderFont(Info, Char2);
	}else if (!Char2){
		bResult = AnsiRenderFont(Info, Char1);
	}else if (Char1 == '1'){		/* pcansi */
		bResult = AnsiRenderFont(Info, Char2);
	}else if (Char1 == '2'){		/* clear */
		bResult = AnsiRenderClear(Info, Char2);
	}else if (Char1 == '3'){		/* foreground color */
		bResult = AnsiRenderForeground(Info, Char2);
	}else if (Char1 == '4'){		/* background color */
		bResult = AnsiRenderBackground(Info, Char2);
//	}else if (Char1 == '5'){		/* steady (not blinking) */
//	}else if (Char1 == '9'){		/* future use (strike-out/cross) */
	}
	return(bResult);
}
BOOL 
AnsiSelectGraphicRendition(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, LPSTR Buffer, LONG Size)
{
	CHAR Char1 = 0;
	CHAR Char2 = 0;
	CHAR Char3 = 0;				/* 8-bit color mode (alpine.exe) */
	CHAR C;
	LPSTR psz = Buffer;

	while (Size--){
		C = *Buffer++;
		if (C == '['){			/* skip CSI */
			continue;
		}else if (C == ';' || !Size){
			if (!AnsiRender(Info, Char1, Char2, Char3)){
//				msvc_printf("{%s}", psz);
				return(TRUE);	/* ignore */
			}
			Char1 = Char2 = Char3 = 0;
			psz = Buffer;
		}else if (!Char1){
			Char1 = C;
		}else if (!Char2){
			Char2 = C;
		}else if (!Char3){
			Char3 = C;
		}
	}
	if (__CTTY->RVideo){
		Info->wAttributes = AnsiInvertAttrib(Info);
	}
	return(SetConsoleTextAttribute(Handle, Info->wAttributes));
}

/****************************************************/

BOOL 
AnsiScrollVertical(HANDLE Handle, SMALL_RECT *Rect, WORD Attribs, WORD Delta)
{
	CHAR_INFO cInfo;
	COORD cPos;

	cPos.X = Rect->Left;
	cPos.Y = Rect->Top;
	if (Delta > 0){
		cPos.Y += Delta;
		Rect->Bottom -= Delta;
	}else{
		Rect->Top -= Delta;
	}
	cInfo.Char.AsciiChar = ' ';
	cInfo.Attributes = Attribs;
	return(ScrollConsoleScreenBuffer(Handle, Rect, NULL, cPos, &cInfo));
}
BOOL 
AnsiScrollDown(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD Delta)
{
	CHAR_INFO cInfo;
	COORD cPos;
	SMALL_RECT sRect = Info->srWindow;

	cPos.X = sRect.Left;
	cPos.Y = sRect.Top;
	if (Delta > 0){
		cPos.Y += Delta;
		sRect.Bottom -= Delta;
	}else{
		sRect.Top -= Delta;
	}
	cInfo.Char.AsciiChar = ' ';
	cInfo.Attributes = Info->wAttributes;
	return(ScrollConsoleScreenBuffer(Handle, &sRect, NULL, cPos, &cInfo));
}
BOOL 
AnsiDeleteCharacter(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, UINT Count)
{
	return(AnsiScrollHorizontal(Handle, Info, Count, 0));
}
BOOL 
AnsiInsertCharacter(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, UINT Count)
{
	return(AnsiScrollHorizontal(Handle, Info, 0, Count));
}
BOOL 
AnsiInsertLine(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, UINT Count)
{
	SMALL_RECT sRect = Info->srWindow;
	BOOL bResult = FALSE;

	sRect.Top = sRect.Bottom - (Count - 1);
	if (AnsiEraseRect(Handle, sRect, Info->wAttributes)){
		Info->srWindow.Top = Info->dwCursorPosition.Y;
		bResult = AnsiScrollDown(Handle, Info, Count);
	}
	return(bResult);
}
BOOL 
AnsiDeleteLine(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, UINT Count)
{
	CHAR_INFO cInfo;
	COORD cPos = {0, Info->dwCursorPosition.Y};
	SMALL_RECT sRect = Info->srWindow;

	sRect.Top = Info->dwCursorPosition.Y + Count;
	cInfo.Char.AsciiChar = ' ';
	cInfo.Attributes = Info->wAttributes;
	return(ScrollConsoleScreenBuffer(Handle, &sRect, NULL, cPos, &cInfo));
}
BOOL 
AnsiEraseInLine(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, CHAR Char)
{
	DWORD dwCount;
	COORD cPos = {0, Info->dwCursorPosition.Y};
	DWORD dwSize = 0;

	if (!Char || Char == '0'){
		cPos.X = Info->dwCursorPosition.X;
		dwSize = (Info->srWindow.Right - cPos.X) + 1;
	}else if (Char == '1'){
		dwSize = Info->dwCursorPosition.X + 1;
	}else if (Char == '2'){
		dwSize = Info->srWindow.Right + 1;
	}else{
		return(FALSE);
	}
	FillConsoleOutputAttribute(Handle, Info->wAttributes, dwSize, cPos, &dwCount);
	return(FillConsoleOutputCharacter(Handle, ' ', dwSize, cPos, &dwCount));
}
BOOL 
AnsiEraseCharacter(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, DWORD Size)
{
	DWORD dwCount;

	FillConsoleOutputAttribute(Handle, Info->wAttributes, Size, Info->dwCursorPosition, &dwCount);
	return(FillConsoleOutputCharacter(Handle, ' ', Size, Info->dwCursorPosition, &dwCount));
}
BOOL 
AnsiCursorBack(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, UINT Count)
{
	COORD cPos = {Info->dwCursorPosition.X - Count, Info->dwCursorPosition.Y};

	/* termcap - bw: automatic left margin (top.exe) */
	if (cPos.X < 0){
		cPos.X += Info->srWindow.Right + 1;
		cPos.Y--;
	}
	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
AnsiCursorPosition(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, WORD Y, WORD X)
{
	COORD cPos = {X - 1, Y - 1};

	cPos.Y += Info->srWindow.Top;
	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
AnsiNextPage(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, UINT Count)
{
	COORD cPos = {Info->srWindow.Left, Info->srWindow.Top};
	SHORT sSize = Info->srWindow.Bottom - Info->srWindow.Top;

	cPos.Y += sSize * Count;
	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
AnsiCursorHorizontalAbsolute(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, SHORT X)
{
	COORD cPos = {X - 1, Info->dwCursorPosition.Y};

	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
AnsiSaveCursor(CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	COORD cPos = Info->dwCursorPosition;

	__CTTY->Cursor.X = cPos.X;
	__CTTY->Cursor.Y = cPos.Y - Info->srWindow.Top;
	return(TRUE);
}
BOOL 
AnsiRestoreCursor(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	COORD cPos = __CTTY->Cursor;

	cPos.Y += Info->srWindow.Top;
	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
AnsiVerticalPositionAbsolute(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info, SHORT Y)
{
	COORD cPos = {Info->dwCursorPosition.X, Y - 1};

	cPos.Y += Info->srWindow.Top;
	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
AnsiDeviceStatusReport(CONSOLE_SCREEN_BUFFER_INFO *Info, CHAR Parm, CHAR *Result)
{
	BOOL bResult = TRUE;
	DWORD dwCount;
	COORD cPos = Info->dwCursorPosition;

	if (Parm == '6'){	/* CPR (cursor position: vim.exe sends this at start) */
		msvc_sprintf(Result, "\e[%d;%dR", (cPos.Y - Info->srWindow.Top) + 1, cPos.X + 1);
//		SetEvent(__Event);
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
AnsiSetMode(WIN_TTY *Terminal, WORD Arg)
{
	BOOL bResult = TRUE;

	if (Arg == 4){			/* IRM */
		Terminal->Mode.Input |= ENABLE_INSERT_MODE;
	}else if (Arg == 7){		/* VEM */
		Terminal->VEdit = TRUE;
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
AnsiResetMode(WIN_TTY *Terminal, WORD Arg)
{
	BOOL bResult = TRUE;

	if (Arg == 4){
		Terminal->Mode.Input &= ~ENABLE_INSERT_MODE;
	}else if (Arg == 7){
		Terminal->VEdit = FALSE;
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
AnsiDeviceAttributes(HANDLE Handle)
{
	msvc_sprintf(__INPUT_BUF, "\e[?64;22c");
	__Input = __INPUT_BUF;
}

/****************************************************/

BOOL 
AnsiControl(HANDLE Handle, CHAR C, CONSOLE_SCREEN_BUFFER_INFO *Info, SEQUENCE *Seq, DWORD Size)
{
	BOOL bResult = FALSE;

	switch (C){
		case '@':		/* ICH */
			bResult = AnsiInsertCharacter(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'A':		/* CUU */
			bResult = AnsiCursorMove(Handle, Info, -AnsiStrToInt(Seq->Args), 0);
			break;
		case 'B':		/* CUD */
			bResult = AnsiCursorMove(Handle, Info, AnsiStrToInt(Seq->Args), 0);
			break;
		case 'C':		/* CUF */
			bResult = AnsiCursorMove(Handle, Info, 0, AnsiStrToInt(Seq->Args));
			break;
		case 'D':		/* CUB */
			bResult = AnsiCursorBack(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'G':		/* CHA (nano.exe) */
			bResult = AnsiCursorHorizontalAbsolute(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'H':		/* CUP */
			bResult = AnsiCursorPosition(Handle, Info, Seq->Arg1, AnsiStrToInt(Seq->Args));
			break;
		case 'J':		/* ED */
			bResult = AnsiEraseInDisplay(Handle, Info, Seq->Char1);
			break;
		case 'K':		/* EL */
			bResult = AnsiEraseInLine(Handle, Info, Seq->Char1);
			break;
		case 'L':		/* IL */
			bResult = AnsiInsertLine(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'M':		/* DL */
			bResult = AnsiDeleteLine(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'P':		/* DCH */
			bResult = AnsiDeleteCharacter(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'S':		/* SU */
			bResult = AnsiScrollDown(Handle, Info, -1);
			break;
		case 'T':		/* SD */
			bResult = AnsiScrollDown(Handle, Info, 1);
			break;
		case 'U':		/* NP */
			bResult = AnsiNextPage(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'X':		/* ECH */
			bResult = AnsiEraseCharacter(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case ANSI_REP:		/* REP (nano.exe) */
			bResult = AnsiRepeat(Handle, __Char, AnsiStrToInt(Seq->Args));
			break;
		case 'c':		/* DA */
			bResult = AnsiDeviceAttributes(Handle);
			break;
		case 'd':		/* VPA (nano.exe) */
			bResult = AnsiVerticalPositionAbsolute(Handle, Info, AnsiStrToInt(Seq->Args));
			break;
		case 'f':		/* HVP (Horizontal Vertical Position) */
			bResult = AnsiCursorPosition(Handle, Info, Seq->Arg1, AnsiStrToInt(Seq->Args));
			break;
		case 'h':		/* SM */
			bResult = AnsiSetMode(__CTTY, Seq->Arg1);
			break;
		case 'l':		/* RM */
			bResult = AnsiResetMode(__CTTY, Seq->Arg1);
			break;
		case 'm':		/* SGR */
			bResult = AnsiSelectGraphicRendition(Handle, Info, Seq->Buf, Size);
			break;
		case 'n':		/* DSR */
			bResult = AnsiDeviceStatusReport(Info, Seq->Char1, __INPUT_BUF);
			break;
		case 's':		/* SC */
			bResult = AnsiSaveCursor(Info);
			break;
		case 'u':		/* RC */
//			bResult = AnsiRestoreCursor(Handle, Info);
			bResult = TRUE;
			break;
		case ANSI_VEM:		/* VEM */
			bResult = AnsiVerticalEditingMode(Handle, AnsiStrToInt(Seq->Args));
			break;
	}
	return(bResult);
}