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

DWORD 
ScreenMode(WIN_TERMIO *Attribs)
{
	DWORD dwResult = ENABLE_PROCESSED_OUTPUT; // | ENABLE_WRAP_AT_EOL_OUTPUT;
	UINT uiFlags = WIN_OPOST | WIN_ONLCR;

	if ((Attribs->OFlags & uiFlags) != uiFlags){
//		dwResult |= DISABLE_NEWLINE_AUTO_RETURN;	/* Vista xterm */
	}
	if (!Attribs->OFlags){
		dwResult |= ENABLE_WRAP_AT_EOL_OUTPUT;		/* nano */
	}
	return(dwResult);
}
BOOL 
ScreenRenderWindow(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	BOOL bResult = TRUE;
	SMALL_RECT sRect = Info->srWindow;
	COORD cPos = Info->dwCursorPosition;

	if (!GetConsoleScreenBufferInfo(Handle, Info)){
		bResult = FALSE;
	}else if (__CTTY->VEdit){
		Info->srWindow = sRect;
//	}else if (!AnsiEqualRect(&sRect, &Info->srWindow)){
//		vfs_raise(WM_SIZE, Info->srWindow.Right + 1, Info->srWindow.Bottom + 1);
	}else if (cPos.X > sRect.Right){
		Info->dwCursorPosition.X = cPos.X;
	}
	return(bResult);
}
BOOL 
ScreenCarriageReturn(HANDLE Handle, UINT Flags, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	COORD cPos = Info->dwCursorPosition;
	UINT uiFlags = WIN_OPOST | WIN_OCRNL;

	if ((Flags & uiFlags) == uiFlags){
		cPos.Y++;
	}else{
		cPos.X = 0;
	}
	Info->dwCursorPosition = cPos;
	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
ScreenScrollUp(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	SMALL_RECT sRect = Info->srWindow;
	COORD cPos = {0, 0};
	CHAR_INFO cInfo = {' ', Info->wAttributes & 0xFF};

	sRect.Top = 1;
	sRect.Bottom = Info->dwSize.Y - 1;
	return(ScrollConsoleScreenBuffer(Handle, &sRect, NULL, cPos, &cInfo));
}
BOOL 
ScreenLineFeed(HANDLE Handle, UINT Flags, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	SMALL_RECT sRect = Info->srWindow;
	COORD cPos = Info->dwCursorPosition;
	UINT uiFlags = WIN_OPOST | WIN_ONLCR;
	SHORT sBottom = Info->dwSize.Y - 1;

	if ((Flags & uiFlags) == uiFlags){		/* ssh.exe */
		cPos.X = 0;
	}
	if (cPos.Y < sRect.Bottom){
		cPos.Y++;
	}else if (__CTTY->VEdit){
		AnsiScrollUp(Handle, Info, 1);
	}else if (cPos.Y == sBottom){
		ScreenScrollUp(Handle, Info);
	}else{
		cPos.Y++;
		Info->srWindow.Top++;
		Info->srWindow.Bottom++;
	}
	Info->dwCursorPosition = cPos;
	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
ScreenTabulator(HANDLE Handle, UINT Flags, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	DWORD dwCount;
	CONSOLE_SCREEN_BUFFER_INFO csbInfo;

	if (!WriteFile(Handle, "\t", 1, &dwCount, NULL)){
		return(FALSE);
	}else if (GetConsoleScreenBufferInfo(Handle, &csbInfo)){
		Info->dwCursorPosition = csbInfo.dwCursorPosition;
	}
	return(TRUE);
}
VOID 
ScreenControl(HANDLE Handle, UINT Flags, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	DWORD dwCount;

	switch (__Char){
		case 0:		/* Null filler (NUL) */
			break;
		case 1:		/* Start of header (SOH) */
				/* raspbian login (telnet.exe) */
			break;
		case 2:		/* Start of Text (STX) */
			break;
		case 3:		/* End of text (ETX) */
			msvc_sprintf(__INPUT_BUF, "\006");	/* ACK */
			break;
		case 5:		/* Enquiry (ENQ) */
			msvc_sprintf(__INPUT_BUF, "\006");	/* ACK */
			break;
		case 8:		/* Backspace (BS) */
			AnsiCursorBack(Handle, Info, 1);
			break;
		case 9:		/* Tabulator (TAB) */
			ScreenTabulator(Handle, Flags, Info);
			break;
		case 10:	/* Linefeed (LF) */
			ScreenLineFeed(Handle, Flags, Info);
			break;
		case 12:	/* Formfeed (FF) */
			/* ignore form-feed, they are intended for paper output! */
			WriteFile(Handle, "\n", 1, &dwCount, NULL);
			break;
		case 13:	/* Carriage return (CR) */
			ScreenCarriageReturn(Handle, Flags, Info);
			break;
		case 14:	/* Shift-Out (SO): red tape (Switch to an alternative character set) */
			SetConsoleOutputCP(GetOEMCP());
			break;
		case 15:	/* Shift-In (SI): black tape (Return to regular character set after Shift Out) */
			SetConsoleOutputCP(CP_UTF8);
			break;
		default:
			WriteFile(Handle, &__Char, 1, &dwCount, NULL);
	}
}
BOOL 
ScreenAnsi(HANDLE Handle, CHAR C, SEQUENCE *Seq, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	BOOL bResult = FALSE;
	DWORD dwSize = __Escape - Seq->Buf;
	COORD cPos = Info->dwCursorPosition;

	if (Seq->CSI == '['){
		bResult = AnsiControl(Handle, C, Info, Seq, dwSize);

	}else if (Seq->CSI == '?'){		/* DEC private */
		if (C == 'h'){			/* DECSM */
			bResult = DECSetMode(Handle, AnsiStrToInt(Seq->Args));
		}else if (C == 'l'){		/* DECRM */
			bResult = DECResetMode(Handle, AnsiStrToInt(Seq->Args));
		}else if (C == 'u'){		/* DECRSPS */
			bResult = DECRestorePresentationState(Handle, Info, AnsiStrToInt(Seq->Args));
		}

	}else if (!Seq->CSI){			/* DEC private (ANSI allowed) */
		if (C == '7'){			/* DECSC (apt-get) */
			bResult = AnsiSaveCursor(Info);
		}else if (C == '8'){		/* DECRC (apt-get) */
			bResult = AnsiRestoreCursor(Handle, Info);
//		}else if (C == 'c'){		/* RIS */
//			bResult = AnsiResetToInitalState(Handle, Info);
		}

	}else if (Seq->CSI == '('){		/* DEC SCS (Shift-In) */
		bResult = DECSelectCharacterSet(Handle, C);

	}else if (Seq->CSI == ')'){		/* DEC SCS (Shift-Out) */
		bResult = DECResetCharacterSet(Handle, C);

	}
	if (!bResult){
		Seq->Buf[dwSize] = 0;
		msvc_printf("\e%s", Seq->Buf);
	}
	__Escape = NULL;
	return(bResult);
}
VOID 
ScreenEscape(HANDLE Handle, CHAR C, SEQUENCE *Seq, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	*__Escape++ = C;

	if (C == '\a'){			/* end of hyperlink */
		__Escape = NULL;
	}else if (C == '@' || C == '>'){
		ScreenAnsi(Handle, C, Seq, Info);
	}else if (Seq->CSI == ']'){	/* hyperlinks (systemctl) */
		return;			/* ignore */
	}else if (C == ';'){
		Seq->Arg1 = AnsiStrToInt(Seq->Args);
		Seq->Char1 = 0;
		Seq->Args = __Escape;
	}else if (!IsCharAlphaNumeric(C)){
		Seq->Args++;
		Seq->CSI = C;
	}else if (IsCharAlpha(C)){
		ScreenAnsi(Handle, C, Seq, Info);
	}else if (!Seq->CSI){		/* no sequence introducer (apt-get) */
		ScreenAnsi(Handle, C, Seq, Info);
	}else if (!Seq->Char1){
		Seq->Char1 = C;
	}
}
DWORD 
ScreenPutChar(HANDLE Handle, LPCSTR Buffer, UINT Flags, UINT CodePage, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	DWORD dwWidth, dwCount = 1;

	if (Info->dwCursorPosition.X > Info->srWindow.Right){
		Info->dwCursorPosition.X = 0;		/* mutt */
		ScreenLineFeed(Handle, Flags, Info);
	}
	if (CodePage == CP_UTF8){
		if ((__Char & 0xE0) == 0xC0){
			dwCount++;
		}else if ((__Char & 0xF0) == 0xE0){
			dwCount += 2;
		}else if ((__Char & 0xF8) == 0xF0){
			dwCount += 3;
		}else if ((__Char & 0xFC) == 0xF8){
			dwCount += 4;
		}
	}
	if (WriteFile(Handle, Buffer, dwCount, &dwWidth, NULL)){
		Info->dwCursorPosition.X += dwWidth;
	}
	return(dwCount);
}

/****************************************************/

BOOL 
screen_write(HANDLE Handle, LPCSTR Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = TRUE;
	DWORD dwCount;
	DWORD dwResult = 0;
	DWORD dwMode = ScreenMode(&__CTTY->Attribs);
	UINT uiFlags = __CTTY->Attribs.OFlags;
	CONSOLE_SCREEN_BUFFER_INFO *psbInfo = &__CTTY->Info;
	UINT uiCodePage = GetConsoleOutputCP();

	if (dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING){
		bResult = WriteFile(Handle, Buffer, Size, &dwResult, NULL);
	}else if (!ScreenRenderWindow(Handle, psbInfo)){
		bResult = FALSE;
	}else while (dwResult < Size){
		__Char = *Buffer;
		dwCount = 1;
		if (__Escape){
			ScreenEscape(Handle, __Char, &__ANSI_BUF, psbInfo);
		}else if (__Char == '\e'){
			__Escape = __ANSI_BUF.Buf;
			__ANSI_BUF.Arg1 = 1;
			__ANSI_BUF.Char1 = 0;
			__ANSI_BUF.CSI = 0;
			__ANSI_BUF.Args = __Escape;
		}else if (__Char < 32){			/* Space (SP) */
			ScreenControl(Handle, uiFlags, psbInfo);
		}else{
			dwCount = ScreenPutChar(Handle, Buffer, uiFlags, uiCodePage, psbInfo);
		}
		dwResult += dwCount;
		Buffer += dwCount;
	}
	*Result = dwResult;
	return(bResult);
}
DWORD 
screen_poll(HANDLE Handle, WIN_POLLFD *Info)
{
	DWORD dwResult = 0;
	SHORT sResult = Info->Result | WIN_POLLOUT;	/* ssh.exe */

	if (Info->Result = sResult & Info->Events){
		dwResult++;
	}
	return(dwResult);
}

/****************************************************/

BOOL 
screen_TIOCGWINSZ(HANDLE Handle, WIN_WINSIZE *Result)
{
	CONSOLE_SCREEN_BUFFER_INFO sbInfo;
	BOOL bResult = FALSE;

	if (!GetConsoleScreenBufferInfo(Handle, &sbInfo)){
		WIN_ERR("GetConsoleScreenBufferInfo(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else{
		Result->Column = (sbInfo.srWindow.Right - sbInfo.srWindow.Left) + 1;
		Result->Row = (sbInfo.srWindow.Bottom - sbInfo.srWindow.Top) + 1;
		Result->XPixel = sbInfo.dwCursorPosition.X + 1;
		Result->YPixel = sbInfo.dwCursorPosition.Y + 1;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
screen_TIOCSWINSZ(HANDLE Handle, WIN_WINSIZE *WinSize)
{
	BOOL bResult = FALSE;
	COORD cSize = {WinSize->Column, WinSize->Row};
	SMALL_RECT sRect = {0, 0, cSize.X - 1, cSize.Y - 1};

	if (!SetConsoleScreenBufferSize(Handle, cSize)){
		WIN_ERR("SetConsoleScreenBufferSize(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else if (!SetConsoleWindowInfo(Handle, TRUE, &sRect)){
		WIN_ERR("SetConsoleWindowInfo(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
screen_TIOCDRAIN(HANDLE Handle)
{
	return(TRUE);		/* CONOUT$ not buffered */
}
