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

CHAR __MAIL_INPUT[WIN_MAX_INPUT + 1];

CHAR *__Buffer = __MAIL_INPUT;

/****************************************************/

VOID 
TTYCarriageReturn(HANDLE Handle, UINT Flags, OVERLAPPED *ovl)
{
	DWORD dwCount = 0;

	if (Flags & WIN_OCRNL){
		WriteFile(Handle, "\n", 1, &dwCount, ovl);
	}else{
		WriteFile(Handle, "\r", 1, &dwCount, ovl);
	}
}
VOID 
TTYLineFeed(HANDLE Handle, UINT Flags, OVERLAPPED *ovl)
{
	DWORD dwCount = 0;

	if (Flags & WIN_ONLCR){
		WriteFile(Handle, "\r", 1, &dwCount, ovl);
	}
	WriteFile(Handle, "\n", 1, &dwCount, ovl);
}
VOID 
TTYControl(HANDLE Handle, UINT Flags, CHAR C, OVERLAPPED *ovl)
{
	DWORD dwCount = 0;

	switch (C){
		case '\n':
			TTYLineFeed(Handle, Flags, ovl);
			break;
		case '\r':
			TTYCarriageReturn(Handle, Flags, ovl);
			break;
		default:
			WriteFile(Handle, &C, 1, &dwCount, ovl);
	}
}

/****************************************************/

BOOL 
tty_open(WIN_DEVICE *Device, WIN_FLAGS *Flags, WIN_VNODE *Result)
{
	Result->Event = Device->Input;
	Result->Index = Device->Index;
	Result->Device = Device;
//vfs_ktrace("tty_open", STRUCT_DEVICE, Device);
	return(TRUE);
}
BOOL 
tty_close(WIN_TTY *Terminal)
{
	BOOL bResult = FALSE;

	if (pdo_revoke(DEVICE(Terminal->DeviceId))){
		ZeroMemory(Terminal, sizeof(WIN_TTY));
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
tty_write(HANDLE Handle, LPCSTR Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = TRUE;
	LONG lResult = 0;
	DWORD dwCount = 0;
	UCHAR C;
	OVERLAPPED ovl = {0, 0, 0, 0, __MailEvent};

	while (lResult < Size){
		C = *Buffer++;
		if (C < 32){
			TTYControl(Handle, __CTTY->Mode.OFlags, C, &ovl);
		}else{
			WriteFile(Handle, &C, 1, &dwCount, &ovl);
		}
		lResult++;
	}
	*Result = lResult;
	return(bResult);
}
BOOL 
tty_read(HANDLE Handle, LPSTR Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = FALSE;
	LONG lSize = Size;
	DWORD dwResult = 0;
	DWORD dwCount;
	CHAR C;

	while (!bResult){
		if (lSize < 1){
			bResult = TRUE;
		}else if (C = *__Buffer){
			*Buffer++ = C;
			__Buffer++;
			dwResult++;
			lSize--;
		}else if (!ReadFile(Handle, __MAIL_INPUT, WIN_MAX_INPUT, &dwCount, NULL)){
			break;
		}else{
			__MAIL_INPUT[dwCount] = 0;
			__Buffer = __MAIL_INPUT;
		}
	}
	*Result = dwResult;
	return(bResult);
}
