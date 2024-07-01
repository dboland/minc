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
InputMode(WIN_TERMIO *Attribs)
{
	DWORD dwResult = ENABLE_WINDOW_INPUT | ENABLE_INSERT_MODE | 
		ENABLE_QUICK_EDIT_MODE | ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT;

	if (Attribs->LFlags & WIN_ECHO){
		dwResult |= ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT;
	}else if (Attribs->LFlags & WIN_ICANON){
		dwResult |= ENABLE_LINE_INPUT;
	}
	if (Attribs->LFlags & WIN_ISIG){
		dwResult |= ENABLE_PROCESSED_INPUT;
	}
	return(dwResult);
}
VOID 
InputChar(CHAR C, DWORD KeyState, CHAR *Result)
{
	DWORD dwCount;
	UINT uiMessage;

	/* On Schift Out (SO), switch to alternative character set,
	 * for file names on remote systems.
	 */
	if (C == 14){		/* Ctrl-N (SO): shift out */
		SetConsoleOutputCP(GetACP());
	}else if (C == 15){	/* Ctrl-O (SI): shift in */
		SetConsoleOutputCP(CP_UTF8);
	}else if (KeyState & LEFT_ALT_PRESSED){		/* nano.exe */
		*Result++ = '\e';
	}
	*Result++ = C;
	*Result = 0;
}
VOID 
InputReturn(DWORD KeyState, WIN_TERMIO *Attribs, CHAR *Result)
{
	if (Attribs->IFlags & WIN_ICRNL){
		InputChar('\n', KeyState, Result);
	}else{
		InputChar('\r', KeyState, Result);
	}
}
VOID 
InputTabulator(DWORD KeyState, CHAR *Result)
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
	BOOL bResult = TRUE;

	if (!(KeyState & SHIFT_PRESSED)){
		win_strcpy(Result, ANSI_CURSOR(VK_INSERT));
	}else if (!IsClipboardFormatAvailable(CF_TEXT)){
		*Result = 0;
	}else if (!OpenClipboard(NULL)){
		WIN_ERR("OpenClipboard(): %s\n", win_strerror(GetLastError()));
		bResult = FALSE;
	}else if (__Lock = GetClipboardData(CF_TEXT)){
		__Clipboard = GlobalLock(__Lock);
	}
	return(bResult);
}
BOOL 
InputKey(KEY_EVENT_RECORD *Event, WIN_TERMIO *Attribs, CHAR *Result)
{
	BOOL bResult = TRUE;
	CHAR CH = Event->uChar.AsciiChar;
	WORD VK = Event->wVirtualKeyCode;

	if (!Event->bKeyDown){
		return(FALSE);
	}else if (VK == VK_RETURN){
		InputReturn(Event->dwControlKeyState, Attribs, Result);
	}else if (CH){
		InputChar(CH, Event->dwControlKeyState, Result);
	}else if (VK == VK_INSERT){
		bResult = InputInsert(Event->dwControlKeyState, Result);
	}else if (VK <= VK_MODIFY){
		bResult = FALSE;
	}else if (VK <= VK_CURSOR){
		win_strcpy(Result, ANSI_CURSOR(VK));
	}else if (VK <= VK_WINDOWS){
		bResult = FALSE;
	}else if (VK <= VK_FUNCTION){
		win_strcpy(Result, ANSI_FUNCTION(VK));
	}else{
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
		win_strcpy(Result, ANSI_CURSOR(VK_UP));
	}else{
		win_strcpy(Result, ANSI_CURSOR(VK_DOWN));
	}
	return(bResult);
}
BOOL 
InputEvent(INPUT_RECORD *Record, WIN_TERMIO *Attribs, LPSTR Buffer)
{
	BOOL bResult = FALSE;

	switch (Record->EventType){
		case KEY_EVENT:
			bResult = InputKey(&Record->KeyEvent, Attribs, Buffer);
			break;
		case WINDOW_BUFFER_SIZE_EVENT:
		case FOCUS_EVENT:
		case MENU_EVENT:
		case MOUSE_EVENT:
			break;
		default:
			SetLastError(ERROR_IO_DEVICE);
	}
	return(bResult);
}
BOOL 
InputReadEvent(HANDLE Handle, WIN_TERMIO *Attribs, CHAR *Buffer)
{
	BOOL bResult = FALSE;
	INPUT_RECORD iRecord;
	DWORD dwCount = 0;

	*Buffer = 0;
	if (!ReadConsoleInput(Handle, &iRecord, 1, &dwCount)){
		WIN_ERR("ReadConsoleInput(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else{
		bResult = InputEvent(&iRecord, Attribs, Buffer);
	}
	__Input = Buffer;
	return(bResult);
}
BOOL 
InputReadLine(HANDLE Handle, WIN_TERMIO *Attribs, CHAR *Buffer)
{
	BOOL bResult = FALSE;
	LONG lCount = 0;

	*Buffer = 0;
	if (!ReadFile(Handle, Buffer, WIN_MAX_INPUT, &lCount, NULL)){
		vfs_raise(WM_COMMAND, CTRL_ABORT_EVENT, 0);
	}else if (lCount > 0){
		lCount--;
		if (Attribs->LFlags & WIN_ISIG){		/* ENABLE_PROCESSED_INPUT */
			Buffer[lCount--] = 0;		/* remove NL, leave CR */
		}
		if (Attribs->IFlags & WIN_ICRNL){
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
BOOL 
InputIsAnsi(KEY_EVENT_RECORD *Event)
{
	BOOL bResult = FALSE;
	UCHAR CH = Event->uChar.AsciiChar;
	WORD VK = Event->wVirtualKeyCode;

	if (!Event->bKeyDown){
		return(FALSE);
	}else if (CH){
		bResult = TRUE;
	}else if (VK <= VK_MODIFY){
		bResult = FALSE;
	}else if (VK <= VK_CURSOR){
		bResult = *ANSI_CURSOR(VK);
	}else if (VK <= VK_WINDOWS){
		bResult = FALSE;
	}else if (VK <= VK_FUNCTION){
		bResult = *ANSI_FUNCTION(VK);
	}
	return(bResult);
}
BOOL 
InputIsEvent(INPUT_RECORD *Record)
{
	BOOL bResult = FALSE;

	switch (Record->EventType){
		case KEY_EVENT:
			bResult = InputIsAnsi(&Record->KeyEvent);
			break;
		case WINDOW_BUFFER_SIZE_EVENT:
		case MOUSE_EVENT:
		case FOCUS_EVENT:
		case MENU_EVENT:
			break;
		default:
			SetLastError(ERROR_IO_DEVICE);
	}
	return(bResult);
}

/****************************************************/

BOOL 
input_read(HANDLE Handle, LPSTR Buffer, DWORD Size, DWORD *Result)
{
	CHAR C = 0;
	BOOL bResult = FALSE;
	DWORD dwResult = 0;
	DWORD dwMode = InputMode(&__CTTY->Attribs);
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
			InputReadLine(Handle, &__CTTY->Attribs, __INPUT_BUF);
		}else{
			InputReadEvent(Handle, &__CTTY->Attribs, __INPUT_BUF);
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
	SHORT sResult = 0;
	SHORT sMask = Info->Events | WIN_POLLIGNORE;

	if (*__Input || __Clipboard){		/* vim.exe */
		sResult = WIN_POLLIN;
	}else if (!PeekConsoleInput(Handle, &iRecord, 1, &dwCount)){
		sResult = WIN_POLLERR;
	}else if (!dwCount){
		sResult = 0;
	}else if (!InputIsEvent(&iRecord)){
		ReadConsoleInput(Handle, &iRecord, 1, &dwCount);
	}else{
		sResult = WIN_POLLIN;
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

	/* "Handle is invalid" if CONIN$ buffer empty
	 */
	if (!FlushConsoleInputBuffer(Handle)){
		WIN_ERR("FlushConsoleInputBuffer(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
