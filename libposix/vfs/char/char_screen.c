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
ScreenCarriageReturn(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	COORD cPos = Info->dwCursorPosition;

	if (__CTTY->Mode[1] & WIN_OCRNL){
		cPos.Y++;
	}else{
		cPos.X = 0;
	}
	return(SetConsoleCursorPosition(Handle, cPos));
}
BOOL 
ScreenLineFeed(HANDLE Handle, CONSOLE_SCREEN_BUFFER_INFO *Info)
{
	BOOL bResult = TRUE;
	COORD cPos = Info->dwCursorPosition;
	SHORT sBottom = Info->dwSize.Y - 1;
	SMALL_RECT sRect = Info->srWindow;

	/* Normally, the controlling TTY handles ONLCR, sending an 
	 * extra CR. Luckily ncurses clears this bit by default, 
	 * on top of setting Vertical Editing Mode (VEM).
	 * So we can handle it here until we have a real CTTY.
	 * With less.exe we're not so lucky. It overrides the ncurses
	 * defaults. This can be solved by exporting "LESS=-XR". The
	 * "R" option is for git.exe, which emits ansi sequences.
	 */
	if (__CTTY->VEdit){
		sBottom = Info->srWindow.Bottom;
	}else if (__CTTY->Mode[1] & WIN_ONLCR){
		cPos.X = 0;
	}
	if (cPos.Y == sBottom){
		sRect.Top = 0;
		AnsiScrollVertical(Handle, &sRect, Info->wAttributes, -1);
	}else{
		cPos.Y++;
	}
	return(SetConsoleCursorPosition(Handle, cPos));
}
VOID 
ScreenControl(HANDLE Handle, CHAR C)
{
	DWORD dwCount;
	CONSOLE_SCREEN_BUFFER_INFO csbInfo;

	if (!GetConsoleScreenBufferInfo(Handle, &csbInfo)){
		__PRINTF("[%d]", C)
	}else switch (C){
		case 0:	/* Null filler (NUL) */
			break;
		case 1:	/* Start of header (SOH) */
			/* raspbian login (telnet.exe) */
			break;
		case 2:	/* Start of Text (STX) */
			break;
		case 3:	/* End of text (ETX) */
			msvc_sprintf(__INPUT_BUF, "\006");	/* ACK */
			break;
		case 5:	/* Enquiry (ENQ) */
			msvc_sprintf(__INPUT_BUF, "\006");	/* ACK */
			break;
		case 8:	/* Backspace (BS) */
			AnsiCursorBack(Handle, &csbInfo, 1);
			break;
		case 10:	/* Linefeed (LF) */
			ScreenLineFeed(Handle, &csbInfo);
			break;
		case 12:	/* Formfeed (FF) */
			/* ignore form-feed, they are intended for paper output! */
			WriteFile(Handle, "\n", 1, &dwCount, NULL);
			break;
		case 13:	/* Carriage return (CR) */
			ScreenCarriageReturn(Handle, &csbInfo);
			break;
		case 14:	/* Shift-Out (SO): red tape (Switch to an alternative character set) */
			SetConsoleOutputCP(GetOEMCP());
			break;
		case 15:	/* Shift-In (SI): black tape (Return to regular character set after Shift Out) */
			SetConsoleOutputCP(CP_UTF8);
			break;
		default:
			WriteFile(Handle, &C, 1, &dwCount, NULL);
	}
}
BOOL 
ScreenAnsi(HANDLE Handle, CHAR C, SEQUENCE *Seq)
{
	BOOL bResult = FALSE;
	CONSOLE_SCREEN_BUFFER_INFO csbInfo;
	DWORD dwSize = __Escape - Seq->Buf;

	GetConsoleScreenBufferInfo(Handle, &csbInfo);

	if (Seq->CSI == '['){
		bResult = AnsiControl(Handle, C, &csbInfo, Seq, dwSize);

	}else if (Seq->CSI == '?'){		/* DEC private */
		if (C == 'h'){			/* DECSM */
			bResult = DECSetMode(Handle, __CTTY, AnsiStrToInt(Seq->Args));
		}else if (C == 'l'){		/* DECRM */
			bResult = DECResetMode(Handle, __CTTY, AnsiStrToInt(Seq->Args));
		}

	}else if (!Seq->CSI){			/* DEC private (ANSI allowed) */
		if (C == '7'){			/* DECSC */
			bResult = DECSaveCursor(&csbInfo);
		}else if (C == '8'){		/* DECRC */
			bResult = DECRestoreCursor(Handle, &csbInfo);
//		}else if (C == 'r'){		/* DECSTBM */
		}else if (C == 'c'){		/* RIS */
			bResult = DECResetInitialState(Handle);
		}

	}else if (Seq->CSI == '('){		/* DEC SCS (Shift-In) */
		bResult = DECSelectCharacterSet(Handle, C);


	}else if (Seq->CSI == ')'){		/* DEC SCS (Shift-Out) */
		bResult = DECResetCharacterSet(Handle, C);

	}

	if (!bResult){
		Seq->Buf[dwSize] = 0;
		msvc_printf("{%s}", Seq->Buf);
	}

	__Escape = NULL;
	return(bResult);
}
VOID 
ScreenEscape(HANDLE Handle, CHAR C, SEQUENCE *Seq)
{
	*__Escape++ = C;

	if (C == '\e'){
		msvc_printf("%s", Seq->Buf);
	}else if (C == '@' || C == '>'){
		ScreenAnsi(Handle, C, Seq);
	}else if (C == ';'){
		Seq->Arg1 = AnsiStrToInt(Seq->Args);
		Seq->Char1 = 0;
		Seq->Args = __Escape;
	}else if (!IsCharAlphaNumeric(C)){
		Seq->Args++;
		Seq->CSI = C;
	}else if (IsCharAlpha(C)){
		ScreenAnsi(Handle, C, Seq);
	}else if (!Seq->CSI){		/* XTERM/DEC private (apt-get on Linux) */
		ScreenAnsi(Handle, C, Seq);
	}else if (!Seq->Char1){
		Seq->Char1 = C;
	}
}
BOOL 
ScreenMultiByte(HANDLE Handle, LPCSTR Buffer, DWORD *Result)
{
	BOOL bResult = FALSE;
	DWORD dwResult, dwCount = 1;

	if ((__Char & 0xE0) == 0xC0){
		dwCount++;
	}else if ((__Char & 0xF0) == 0xE0){
		dwCount += 2;
	}else if ((__Char & 0xF8) == 0xF0){
		dwCount += 3;
	}else if ((__Char & 0xFC) == 0xF8){
		dwCount += 4;
	}
	if (WriteFile(Handle, Buffer, dwCount, &dwResult, NULL)){
		*Result = dwCount;	/* return actual, not computed */
		bResult = TRUE;
	}
	return(bResult);
}

/****************************************************/

BOOL 
screen_TIOCGWINSZ(HANDLE Handle, WIN_WINSIZE *WinSize)
{
	CONSOLE_SCREEN_BUFFER_INFO sbInfo;
	BOOL bResult = FALSE;

	if (!GetConsoleScreenBufferInfo(Handle, &sbInfo)){
		WIN_ERR("GetConsoleScreenBufferInfo(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else{
		WinSize->Column = (sbInfo.srWindow.Right - sbInfo.srWindow.Left) + 1;
		WinSize->Row = (sbInfo.srWindow.Bottom - sbInfo.srWindow.Top) + 1;
		WinSize->XPixel = sbInfo.dwCursorPosition.X + 1;
		WinSize->YPixel = sbInfo.dwCursorPosition.Y + 1;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
screen_TIOCDRAIN(HANDLE Handle)
{
	return(TRUE);		/* CONOUT$ not buffered */
}

/****************************************************/

BOOL 
screen_write(HANDLE Handle, LPCSTR Buffer, DWORD Size, DWORD *Result)
{
	DWORD dwCount;
	BOOL bResult = TRUE;
	DWORD dwResult = 0;
	DWORD dwMode = __CTTY->Mode[1];
	UINT uiCodePage = GetConsoleOutputCP();

	if (dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING){
		bResult = WriteFile(Handle, Buffer, Size, &dwResult, NULL);
	}else while (dwResult < Size){
		__Char = *Buffer;
		dwCount = 1;
		if (__Escape){
			ScreenEscape(Handle, __Char, &__ANSI_BUF);
		}else if (__Char == '\e'){
			__Escape = __ANSI_BUF.Buf;
			__ANSI_BUF.Arg1 = 1;
			__ANSI_BUF.Char1 = 0;
			__ANSI_BUF.CSI = 0;
			__ANSI_BUF.Args = __Escape;
		}else if (__Char < 32){			/* Space (SP) */
			ScreenControl(Handle, __Char);
		}else if (uiCodePage == CP_UTF8){
			ScreenMultiByte(Handle, Buffer, &dwCount);
		}else{
			WriteFile(Handle, Buffer, 1, &dwCount, NULL);
		}
		dwResult += dwCount;
		Buffer += dwCount;
	}
	*Result = dwResult;
	return(bResult);
}
DWORD 
screen_poll(HANDLE Screen, WIN_POLLFD *Info)
{
	DWORD dwResult = 0;
	SHORT sResult = Info->Result | WIN_POLLOUT;	/* ssh.exe */

	if (Info->Result = sResult & Info->Events){
		dwResult++;
	}
	return(dwResult);
}
