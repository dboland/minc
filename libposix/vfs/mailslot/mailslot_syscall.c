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

HANDLE 
MailOpenFile(LPCSTR FileName, SECURITY_ATTRIBUTES *Security)
{
	HANDLE hResult = NULL;

	hResult = CreateFile(FileName, GENERIC_WRITE | READ_CONTROL, FILE_SHARE_READ, 
		Security, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hResult == INVALID_HANDLE_VALUE){
		WIN_ERR("CreateFile(%s): %s\n", FileName, win_strerror(GetLastError()));
	}
	return(hResult);
}
BOOL 
MailCreateDevice(LPCSTR FileName, SECURITY_ATTRIBUTES *Security, WIN_DEVICE *Result)
{
	BOOL bResult = FALSE;
	HANDLE hResult = NULL;

	hResult = CreateMailslot(FileName, WIN_MAX_INPUT, MAILSLOT_WAIT_FOREVER, Security);
	if (hResult == INVALID_HANDLE_VALUE){
		WIN_ERR("CreateMailslot(%s): %s\n", FileName, win_strerror(GetLastError()));
	}else{
		Result->Input = hResult;
		Result->Output = MailOpenFile(FileName, Security);
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
MailCreateSlave(WIN_DEVICE *Device, HANDLE Event, SECURITY_ATTRIBUTES *Security, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	CHAR szPath[MAX_PATH] = "\\\\.\\MAILSLOT\\slave\\";
	HANDLE hResult = NULL;

	if (MailCreateDevice(win_strcat(szPath, Device->Name), Security, Device)){
		Result->Event = Event;
		Result->FSType = Device->FSType;
		Result->FileType = Device->FileType;
		Result->DeviceType = Device->DeviceType;
		Result->DeviceId = Device->DeviceId;
		Result->Attribs = FILE_ATTRIBUTE_PDO | FILE_ATTRIBUTE_DEVICE;
		Result->Access = win_F_GETFL(Device->Input);
		Result->Flags = win_F_GETFD(Device->Input);
		Result->Device = Device;
		bResult = TRUE;
	}
	return(bResult);
}
