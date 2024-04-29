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

#include <winbase.h>

/****************************************************/

VOID 
TTYCarriageReturn(HANDLE Handle, DWORD Mode, OVERLAPPED *ovl)
{
	DWORD dwCount = 0;

	if (Mode & WIN_OCRNL){
		WriteFile(Handle, "\n", 1, &dwCount, ovl);
	}else{
		WriteFile(Handle, "\r", 1, &dwCount, ovl);
	}
}
VOID 
TTYLineFeed(HANDLE Handle, DWORD Mode, OVERLAPPED *ovl)
{
	DWORD dwCount = 0;

	if (Mode & WIN_ONLCR){
		WriteFile(Handle, "\r", 1, &dwCount, ovl);
	}
	WriteFile(Handle, "\n", 1, &dwCount, ovl);
}
VOID 
TTYControl(HANDLE Handle, DWORD Mode, CHAR C, OVERLAPPED *ovl)
{
	DWORD dwCount = 0;

	switch (C){
		case '\n':
			TTYLineFeed(Handle, Mode, ovl);
			break;
		case '\r':
			TTYCarriageReturn(Handle, Mode, ovl);
			break;
		default:
			WriteFile(Handle, &C, 1, &dwCount, ovl);
	}
}

/****************************************************/
VOID 
tty_init(VOID)
{
	DWORD dwIndex = 0;
	WIN_DEVICE *pwDevice = DEVICE(DEV_TYPE_TTY);

	while (dwIndex < WIN_TTY_MAX){
		pwDevice->Index = dwIndex++;
		pwDevice++;
	}
}
WIN_TTY * 
tty_attach(WIN_DEVICE *Device)
{
	DWORD dwIndex = 0;
	WIN_TTY *pwTerminal = &__Terminals[dwIndex];

	while (dwIndex < WIN_TTY_MAX){
		if (!pwTerminal->Flags){
			pwTerminal->Flags = TIOCFLAG_ACTIVE;
			pwTerminal->DeviceId = Device->DeviceId;
			pwTerminal->TerminalId = dwIndex;
			pwTerminal->ScrollRate = 1;
			Device->Index = dwIndex;
			return(pwTerminal);
		}
		dwIndex++;
		pwTerminal++;
	}	
	WIN_ERR("tty_attach(%d): %s\n", dwIndex, win_strerror(ERROR_NOT_ENOUGH_MEMORY));
	vfs_raise(WM_COMMAND, CTRL_ABORT_EVENT, 0);
	return(NULL);
}
BOOL 
tty_open(WIN_DEVICE *Device, WIN_FLAGS *Flags, WIN_VNODE *Result)
{
	Result->Event = Device->Input;
	Result->Index = Device->Index;
	Result->Device = Device;
//vfs_ktrace("tty_open", STRUCT_VNODE, Result);
//	return(config_activate(Device, Result));
	return(TRUE);
}
/* BOOL 
tty_close(WIN_TTY *Terminal)
{
	BOOL bResult = FALSE;

	if (pdo_revoke(DEVICE(Terminal->DeviceId))){
		ZeroMemory(Terminal, sizeof(WIN_TTY));
		bResult = TRUE;
	}
	return(bResult);
} */
BOOL 
tty_write(WIN_DEVICE *Device, LPCSTR Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = TRUE;
	LONG lResult = 0;
	DWORD dwCount = 0;
	UCHAR C;
	OVERLAPPED ovl = {0, 0, 0, 0, __MailEvent};

	while (lResult < Size){
		C = *Buffer++;
		if (C < 32){
			TTYControl(Device->Output, __CTTY->Mode.Output, C, &ovl);
		}else{
			WriteFile(Device->Output, &C, 1, &dwCount, &ovl);
		}
		lResult++;
	}
	*Result = lResult;
	return(bResult);
}
BOOL 
tty_read(WIN_DEVICE *Device, LPSTR Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = FALSE;

	if (Size > WIN_MAX_INPUT){
		Size = WIN_MAX_INPUT;
	}
	if (!ReadFile(Device->Input, Buffer, Size, Result, NULL)){
		vfs_raise(WM_COMMAND, CTRL_ABORT_EVENT, 0);
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
