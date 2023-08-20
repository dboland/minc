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

BOOL 
dev_F_DUPFD(WIN_DEVICE *Device, HANDLE Process, DWORD Options, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	HANDLE hDevice = NULL;
	HANDLE hResult = NULL;

	if (!Result->FileId){
		hDevice = Device->Input;
	}else{
		hDevice = Device->Output;
	}
	if (!hDevice){
		SetLastError(ERROR_IO_DEVICE);
	}else if (!DuplicateHandle(GetCurrentProcess(), hDevice, Process, &hResult, 0, TRUE, Options)){
		WIN_ERR("dev_F_DUPFD(%d): %s\n", hDevice, win_strerror(GetLastError()));
	}else{
		Result->Handle = hResult;
		Result->FSType = Device->FSType;
		Result->FileType = Device->FileType;
		Result->DeviceType = Device->DeviceType;
		Result->DeviceId = Device->DeviceId;
		Result->Device = Device;
		bResult = TRUE;
	}
	return(bResult);
}

/****************************************************/

BOOL 
dev_open(WIN_NAMEIDATA *Path, WIN_FLAGS *Flags, WIN_MODE *Mode, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;

	if (!DevOpenFile(Path, Flags, Result)){
		return(FALSE);
	}else if (!Result->Device->Flags){
		SetLastError(ERROR_DEVICE_NOT_AVAILABLE);
	}else switch (Result->DeviceType){
		case DEV_TYPE_CONSOLE:
			bResult = dev_activate(Result->Device, Result);
			break;
		case DEV_CLASS_TTY:
			bResult = tty_open(DEVICE(__CTTY->DeviceId), Flags, Result);
			break;
		case DEV_TYPE_NULL:
			bResult = null_open(Result->Device, Flags, Result);
			break;
//		case DEV_TYPE_INPUT:
//			bResult = char_open("CONIN$", Flags, Result);
//			break;
//		case DEV_TYPE_SCREEN:
//			bResult = char_open("CONOUT$", Flags, Result);
//			break;
		case DEV_TYPE_PTM:
			bResult = pty_open(dev_attach(DEV_TYPE_PTY), Flags, Result);
			break;
		case DEV_TYPE_STDIN:		/* sort.exe */
			bResult = vfs_namei(GetStdHandle(STD_INPUT_HANDLE), 0, Result);
			break;
		case DEV_TYPE_STDOUT:
			bResult = vfs_namei(GetStdHandle(STD_OUTPUT_HANDLE), 1, Result);
			break;
		case DEV_TYPE_STDERR:
			bResult = vfs_namei(GetStdHandle(STD_ERROR_HANDLE), 2, Result);
			break;
		default:
			bResult = TRUE;
	}
	return(bResult);
}
