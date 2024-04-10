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

VOID 
InputChar(CHAR C, DWORD KeyState, CHAR *Result)
{
	DWORD dwCount;
	UINT uiMessage;

	/* On Schift Out (SO), switch to alternative character set,
	 * instead of graphical one (for file names on remote systems).
	 */
	if (C == 14){		/* Ctrl-N (SO): shift out */
		SetConsoleOutputCP(GetACP());
	}else if (C == 15){	/* Ctrl-O (SI): shift in */
		SetConsoleOutputCP(CP_UTF8);
	}else if (KeyState & LEFT_ALT_PRESSED){
		*Result++ = '\e';
	}
	*Result++ = C;
	*Result = 0;
}
VOID 
InputReturn(DWORD KeyState, CHAR *Result)
{
	if (__CTTY->Mode.Input & WIN_ICRNL){
		InputChar('\n', KeyState, Result);
	}else{
		InputChar('\r', KeyState, Result);
	}
}
VOID 
InputTab(DWORD KeyState, CHAR *Result)
{
	if (KeyState & SHIFT_PRESSED){
		win_strcpy(Result, "\e[Z");
	}else{
		InputChar('\t', KeyState, Result);
	}
}
BOOL 
InputInsert(DWORD KeyState, CHAR *Result)
{
	BOOL bResult = FALSE;

	if (!(KeyState & SHIFT_PRESSED)){
		win_strcpy(Result, ANSI_INSERT);
		bResult = TRUE;
	}else if (!IsClipboardFormatAvailable(CF_TEXT)){
		*Result = 0;
	}else if (!OpenClipboard(NULL)){
		WIN_ERR("OpenClipboard(): %s\n", win_strerror(GetLastError()));
	}else if (__Lock = GetClipboardData(CF_TEXT)){
		__Clipboard = GlobalLock(__Lock);
	}
	return(bResult);
}
BOOL 
InputKey(KEY_EVENT_RECORD *Event, CHAR *Result)
{
	UCHAR CH = Event->uChar.AsciiChar;
	WORD VK = Event->wVirtualKeyCode;
	BOOL bResult = TRUE;
	DWORD dwCount;

	if (!Event->bKeyDown){
		return(FALSE);
	}else if (VK == VK_RETURN){
		InputReturn(Event->dwControlKeyState, Result);
	}else if (VK == VK_TAB){
		InputTab(Event->dwControlKeyState, Result);
	}else if (VK == VK_INSERT){
		bResult = InputInsert(Event->dwControlKeyState, Result);
	}else if (CH){
		InputChar(CH, Event->dwControlKeyState, Result);
	}else if (VK == VK_PRIOR){
		win_strcpy(Result, ANSI_PRIOR);
	}else if (VK == VK_NEXT){
		win_strcpy(Result, ANSI_NEXT);
	}else if (VK == VK_END){
		win_strcpy(Result, ANSI_END);
	}else if (VK == VK_HOME){
		win_strcpy(Result, ANSI_HOME);
	}else if (VK == VK_LEFT){
		win_strcpy(Result, ANSI_LEFT);
	}else if (VK == VK_UP){
		win_strcpy(Result, ANSI_UP);
	}else if (VK == VK_RIGHT){
		win_strcpy(Result, ANSI_RIGHT);
	}else if (VK == VK_DOWN){
		win_strcpy(Result, ANSI_DOWN);
	}else if (VK == VK_DELETE){
		win_strcpy(Result, ANSI_DELETE);
	}else if (VK == VK_F1){
		win_strcpy(Result, ANSI_F1);
	}else if (VK == VK_F2){
		win_strcpy(Result, ANSI_F2);
	}else if (VK == VK_F3){
		win_strcpy(Result, ANSI_F3);
	}else if (VK == VK_F4){
		win_strcpy(Result, ANSI_F4);
	}else if (VK == VK_F5){
		win_strcpy(Result, ANSI_F5);
	}else if (VK == VK_F6){
		win_strcpy(Result, ANSI_F6);
	}else if (VK == VK_F7){
		win_strcpy(Result, ANSI_F7);
	}else if (VK == VK_F8){
		win_strcpy(Result, ANSI_F8);
	}else if (VK == VK_F9){
		win_strcpy(Result, ANSI_F9);
	}else if (VK == VK_F10){
		win_strcpy(Result, ANSI_F10);
	}else if (VK == VK_F11){
		win_strcpy(Result, ANSI_F11);
	}else if (VK == VK_F12){
		win_strcpy(Result, ANSI_F12);
	}else{
//__PRINTF("[%d:%d:%d]", CH, Event->wVirtualKeyCode, Event->wVirtualScanCode)
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
InputMouse(MOUSE_EVENT_RECORD *Event, CHAR *Result)
{
	BOOL bResult = TRUE;

	if (Event->dwEventFlags != MOUSE_WHEELED){
		bResult = FALSE;
	}else if (GET_WHEEL_DELTA_WPARAM(Event->dwButtonState) > 0){
		win_strcpy(Result, ANSI_UP);
	}else{
		win_strcpy(Result, ANSI_DOWN);
	}
	return(bResult);
}
BOOL 
InputWindow(WINDOW_BUFFER_SIZE_RECORD *Event)
{
	COORD cSize = Event->dwSize;

//	vfs_raise(WM_SIZE, cSize.X, cSize.Y);
	return(FALSE);
}
BOOL 
InputEvent(INPUT_RECORD *Record, LPSTR Buffer)
{
	BOOL bResult = FALSE;

	switch (Record->EventType){
		case KEY_EVENT:
			bResult = InputKey(&Record->KeyEvent, Buffer);
			break;
		case WINDOW_BUFFER_SIZE_EVENT:
			bResult = InputWindow(&Record->WindowBufferSizeEvent);
			break;
		case MOUSE_EVENT:
		case FOCUS_EVENT:
		case MENU_EVENT:
			break;
		default:
			__PRINTF("{%d}", Record->EventType)
	}
	return(bResult);
}
BOOL 
InputReadEvent(HANDLE Handle, CHAR *Buffer)
{
	BOOL bResult = FALSE;
	INPUT_RECORD iRecord;
	DWORD dwCount = 0;

	*Buffer = 0;
	if (!ReadConsoleInput(Handle, &iRecord, 1, &dwCount)){
//		WIN_ERR("ReadConsoleInput(%d): %s\n", Handle, win_strerror(GetLastError()));
		vfs_raise(WM_COMMAND, CTRL_ABORT_EVENT, 0);
	}else{
		bResult = InputEvent(&iRecord, Buffer);
	}
	__Input = Buffer;
	return(bResult);
}
BOOL 
InputReadLine(HANDLE Handle, DWORD Mode, CHAR *Buffer)
{
	BOOL bResult = FALSE;
	LONG lCount = 0;

	*Buffer = 0;
	if (!ReadFile(Handle, Buffer, WIN_MAX_INPUT, &lCount, NULL)){
//		WIN_ERR("InputReadLine(%d): %s\n", Handle, win_strerror(GetLastError()));
		vfs_raise(WM_COMMAND, CTRL_ABORT_EVENT, 0);
	}else if (lCount > 0){
		lCount--;
		if (Mode & ENABLE_PROCESSED_INPUT){
			Buffer[lCount--] = 0;		/* remove NL, leave CR */
		}
		if (Mode & WIN_ICRNL){
			Buffer[lCount] = '\n';		/* replace CR (ftp.exe) */
		}
		bResult = TRUE;
	}
	__Input = Buffer;
	return(bResult);
}
BOOL 
InputReadClipboard(CHAR *Buffer)
{
	BOOL bResult = TRUE;
	LONG lSize = WIN_MAX_INPUT;
	CHAR C;

	__Input = Buffer;
	while (lSize > 0){
		C = *__Clipboard++;
		if (!C){
			GlobalUnlock(__Lock);
			CloseClipboard();
			__Clipboard = NULL;
			break;
		}else if (C != '\n'){
			*Buffer++ = C;
			lSize--;
		}
	}
	*Buffer = 0;
	return(bResult);
}

/****************************************************/

BOOL 
input_read(HANDLE Handle, LPSTR Buffer, DWORD Size, DWORD *Result)
{
	CHAR C = 0;
	BOOL bResult = FALSE;
	DWORD dwResult = 0;
	DWORD dwMode = __CTTY->Mode.Input;
	LONG lSize = Size;

	if (dwMode & ENABLE_VIRTUAL_TERMINAL_INPUT){
		bResult = ReadFile(Handle, Buffer, Size, &dwResult, NULL);
	}else while (!bResult){
		if (lSize < 1){
			bResult = TRUE;
		}else if (C = *__Input){
			*Buffer++ = C;
			dwResult++;
			lSize--;
			__Input++;
		}else if (dwResult){
			bResult = TRUE;
		}else if (__Clipboard){
			InputReadClipboard(__INPUT_BUF);
		}else if (dwMode & ENABLE_LINE_INPUT){
			InputReadLine(Handle, dwMode, __INPUT_BUF);
		}else{
			InputReadEvent(Handle, __INPUT_BUF);
		}
	}
	*Result = dwResult;
	return(bResult);
}
DWORD 
input_poll(HANDLE Handle, WIN_POLLFD *Info)
{
	DWORD dwResult = 0;
	DWORD dwCount = 0;
	INPUT_RECORD iRecord;
	SHORT sResult = WIN_POLLERR;
	SHORT sMask = Info->Events | WIN_POLLIGNORE;

	if (*__Input || __Clipboard){		/* vim.exe */
		sResult = WIN_POLLIN;
	}else if (!PeekConsoleInput(Handle, &iRecord, 1, &dwCount)){
		WIN_ERR("PeekConsoleInput(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else if (!dwCount){
		sResult = 0;
	}else if (InputReadEvent(Handle, __INPUT_BUF)){
		sResult = WIN_POLLIN;
	}else{
		sResult = 0;
	}
	if (Info->Result = sResult & sMask){
		dwResult++;
	}
	return(dwResult);
}

/****************************************************/

BOOL 
input_TIOCFLUSH(HANDLE Handle)
{
	BOOL bResult = FALSE;

	/* "Handle is invalid" if CONIN$ buffer empty */
	if (!FlushConsoleInputBuffer(Handle)){
		WIN_ERR("FlushConsoleInputBuffer(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
