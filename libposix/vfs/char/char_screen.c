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

CONST DWORD 
__CP437[] = {
	0x0, 0xe298ba, 0xe298bb, 0xe299a5, 0xe299a6, 0xe299a3, 0xe299a0, 0xe280a2,
	0xe29798, 0xe2978b, 0xe29799, 0xe29982, 0xe29980, 0xe299aa, 0xe299ab, 0xe298bc,
	0xe296ba, 0xe29784, 0xe28695, 0xe280bc, 0xc2b6, 0xc2a7, 0xe296ac, 0xe286a8,
	0xe28691, 0xe28693, 0xe28692, 0xe28690, 0xe2889f, 0xe28694, 0xe296b2, 0xe296bc,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
	0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
	0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
	0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
	0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0xe28c82,
	0xc387, 0xc3bc, 0xc3a9, 0xc3a2, 0xc3a4, 0xc3a0, 0xc3a5, 0xc3a7,
	0xc3aa, 0xc3ab, 0xc3a8, 0xc3af, 0xc3ae, 0xc3ac, 0xc384, 0xc385,
	0xc389, 0xc3a6, 0xc386, 0xc3b4, 0xc3b6, 0xc3b2, 0xc3bb, 0xc3b9,
	0xc3bf, 0xc396, 0xc39c, 0xc2a2, 0xc2a3, 0xc2a5, 0xe282a7, 0xc692,
	0xc3a1, 0xc3ad, 0xc3b3, 0xc3ba, 0xc3b1, 0xc391, 0xc2aa, 0xc2ba,
	0xc2bf, 0xe28c90, 0xc2ac, 0xc2bd, 0xc2bc, 0xc2a1, 0xc2ab, 0xc2bb,
	0xe29691, 0xe29692, 0xe29693, 0xe29482, 0xe294a4, 0xe295a1, 0xe295a2, 0xe29596,
	0xe29595, 0xe295a3, 0xe29591, 0xe29597, 0xe2959d, 0xe2959c, 0xe2959b, 0xe29490,
	0xe29494, 0xe294b4, 0xe294ac, 0xe2949c, 0xe29480, 0xe294bc, 0xe2959e, 0xe2959f,
	0xe2959a, 0xe29594, 0xe295a9, 0xe295a6, 0xe295a0, 0xe29590, 0xe295ac, 0xe295a7,
	0xe295a8, 0xe295a4, 0xe295a5, 0xe29599, 0xe29598, 0xe29592, 0xe29593, 0xe295ab,
	0xe295aa, 0xe29498, 0xe2948c, 0xe29688, 0xe29684, 0xe2968c, 0xe29690, 0xe29680,
	0xceb1, 0xc39f, 0xce93, 0xcf80, 0xcea3, 0xcf83, 0xc2b5, 0xcf84,
	0xcea6, 0xce98, 0xcea9, 0xceb4, 0xe2889e, 0xcf86, 0xceb5, 0xe288a9,
	0xe289a1, 0xc2b1, 0xe289a5, 0xe289a4, 0xe28ca0, 0xe28ca1, 0xc3b7, 0xe28988,
	0xc2b0, 0xe28899, 0xc2b7, 0xe2889a, 0xe281bf, 0xc2b2, 0xe296a0
};

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
	BOOL bResult = FALSE;

	if (GetConsoleScreenBufferInfo(Handle, Info)){
		Info->srWindow.Left = 0;
		Info->srWindow.Right = Info->dwSize.X - 1;
		bResult = TRUE;
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
		cPos.X = sRect.Left;
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
	DWORD dwResult;

	switch (__Char){
		case 0:		/* Null filler (NUL) */
			break;
		case 1:		/* Start of header (SOH) */
				/* raspbian login (telnet.exe) */
			break;
		case 2:		/* Start of Text (STX) */
			break;
		case 3:		/* End of text (ETX) */
			sprintf(__INPUT_BUF, "\006");	/* ACK */
			break;
		case 5:		/* Enquiry (ENQ) */
			sprintf(__INPUT_BUF, "\006");	/* ACK */
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
			WriteFile(Handle, "\n", 1, &dwResult, NULL);
			break;
		case 13:	/* Carriage return (CR) */
			ScreenCarriageReturn(Handle, Flags, Info);
			break;
		case 14:	/* Shift-Out (SO): red tape (Switch to an alternative character set) */
			SetConsoleOutputCP(437);
			break;
		case 15:	/* Shift-In (SI): black tape (Return to regular character set after Shift Out) */
			SetConsoleOutputCP(CP_UTF8);
			break;
		default:
			WriteFile(Handle, &__Char, 1, &dwResult, NULL);
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
		WIN_ERR("\e%s", Seq->Buf);
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
	}else if (C == '@'){
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
ScreenPutChar(HANDLE Handle, LPCSTR Buffer, UINT Flags, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	DWORD dwWidth, dwCount = 1;
	UCHAR C = Buffer[0];

	if (Info->dwCursorPosition.X > Info->srWindow.Right){
		Info->dwCursorPosition.X = Info->srWindow.Left;		/* mutt.exe */
		ScreenLineFeed(Handle, Flags, Info);
	}
//	if ((C & 0xE0) == 0xC0){
//		dwCount++;
//	}else if ((C & 0xF0) == 0xE0){
//		dwCount += 2;
//	}else if ((C & 0xF8) == 0xF0){
//		dwCount += 3;
//	}else if ((C & 0xFC) == 0xF8){
//		dwCount += 4;
//	}
	/* On Vista, when emitting multibyte (utf-8),
	 * dwWidth will be zero.
	 */
	if (WriteFile(Handle, Buffer, dwCount, &dwWidth, NULL)){
		Info->dwCursorPosition.X++;
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
//	UINT uiCodePage = GetConsoleOutputCP();

	if (dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING){
		bResult = WriteFile(Handle, Buffer, Size, &dwResult, NULL);
	}else if (!ScreenRenderWindow(Handle, psbInfo)){
		return(FALSE);
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
		}else if (__Char < 16){			/* ACS_RARROW (DLE) */
			ScreenControl(Handle, uiFlags, psbInfo);
//		}else if (__Char < 255){
//			dwCount = ScreenPutChar(Handle, (LPSTR)&__CP437[__Char], uiFlags, psbInfo);
		}else{
			dwCount = ScreenPutChar(Handle, Buffer, uiFlags, psbInfo);
		}
		dwResult += dwCount;
		Buffer += dwCount;
	}
	*Result = dwResult;
	return(bResult);
}
BOOL 
screen_poll(HANDLE Handle, WIN_POLLFD *Info, DWORD *Result)
{
	BOOL bResult = TRUE;
	SHORT sResult = Info->Result | WIN_POLLOUT;	/* ssh.exe */

	if (Info->Result = sResult & Info->Events){
		*Result += 1;
	}
	return(bResult);
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
//		Result->Column = (sbInfo.srWindow.Right - sbInfo.srWindow.Left) + 1;
		Result->Column = sbInfo.dwSize.X;
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
