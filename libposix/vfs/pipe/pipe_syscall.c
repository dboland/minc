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
PipeCreateFile(LPCWSTR Name, DWORD Attribs, HANDLE Event, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	ACCESS_MASK aMask = GENERIC_READ + GENERIC_WRITE;
	DWORD dwOpenMode = (Attribs & 0xFFFF0000) + PIPE_ACCESS_DUPLEX;
	DWORD dwPipeMode = (Attribs & 0x0000FFFF) + PIPE_TYPE_MESSAGE + PIPE_WAIT;
	WCHAR szPath[MAX_PATH] = L"\\\\.\\PIPE\\";
	DWORD dwMax = PIPE_UNLIMITED_INSTANCES;
	HANDLE hResult;

	hResult = CreateNamedPipeW(win_wcscat(szPath, Name), dwOpenMode, dwPipeMode, 
		dwMax, WIN_PIPE_BUF, WIN_PIPE_BUF, NMPWAIT_USE_DEFAULT_WAIT, NULL);
	if (hResult != INVALID_HANDLE_VALUE){
		Result->Handle = hResult;
		Result->Event = Event;
		Result->FSType = FS_TYPE_PIPE;
		Result->Attribs = Attribs & 0xFFFF0000;
		Result->Access = win_F_GETFL(hResult);
		Result->Flags = win_F_GETFD(hResult);
		bResult = TRUE;
	}else{
		WIN_ERR("CreateNamedPipe(%ls): %s\n", szPath, win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
PipeOpenFile(LPCWSTR Name, HANDLE Event, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	ACCESS_MASK aMask = GENERIC_WRITE + GENERIC_READ;
	DWORD dwShare = FILE_SHARE_WRITE + FILE_SHARE_READ;
	WCHAR szPath[MAX_PATH] = L"\\\\.\\PIPE\\";
	DWORD dwAttribs = FILE_ATTRIBUTE_NORMAL;
	HANDLE hResult;

	hResult = CreateFileW(win_wcscat(szPath, Name), aMask, dwShare, NULL, 
		OPEN_EXISTING, dwAttribs, NULL);
	if (hResult != INVALID_HANDLE_VALUE){
		Result->Handle = hResult;
		Result->Event = Event;
		Result->FSType = FS_TYPE_PIPE;
		Result->Attribs = dwAttribs;
		Result->Access = win_F_GETFL(hResult);
		Result->Flags = win_F_GETFD(hResult);
		bResult = TRUE;
//	}else{
//		WIN_ERR("PipeOpenFile(%ls): %s\n", szPath, win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
PipeCreateDevice(LPCWSTR Name, DWORD Attribs, WIN_DEVICE *Result)
{
	BOOL bResult = FALSE;
	ACCESS_MASK aMask = GENERIC_READ + GENERIC_WRITE;
	DWORD dwOpenMode = (Attribs & 0xFFFF0000) + PIPE_ACCESS_DUPLEX;
	DWORD dwPipeMode = (Attribs & 0x0000FFFF) + PIPE_TYPE_MESSAGE + PIPE_WAIT;
	CHAR szPath[MAX_PATH] = "\\\\.\\PIPE\\";
	DWORD dwMax = PIPE_UNLIMITED_INSTANCES;
	HANDLE hResult;

	hResult = CreateNamedPipe(win_strcat(szPath, Result->Name), dwOpenMode, dwPipeMode, 
		dwMax, WIN_PIPE_BUF, WIN_PIPE_BUF, NMPWAIT_USE_DEFAULT_WAIT, NULL);
	if (hResult != INVALID_HANDLE_VALUE){
		Result->Input = hResult;
//		Result->Output = PipeOpenFile(szPath);
		Result->Event = CreateEvent(NULL, FALSE, FALSE, NULL);
		bResult = TRUE;
	}else{
		WIN_ERR("CreateNamedPipe(%ls): %s\n", szPath, win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
PipeCreateMaster(WIN_DEVICE *Device, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;

	Result->Handle = Device->Input;
	Result->Event = Device->Event;
	Result->FSType = Device->FSType;
	Result->FileType = Device->FileType;
	Result->DeviceType = Device->DeviceType;
	Result->DeviceId = Device->DeviceId;
	Result->Index = Device->Index;
	bResult = TRUE;
	return(bResult);
}
BOOL 
PipeCreateSlave(WIN_DEVICE *Device, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;

	Result->Handle = Device->Output;
	Result->Event = Device->Event;
	Result->FSType = Device->FSType;
	Result->FileType = Device->FileType;
	Result->DeviceType = Device->DeviceType;
	Result->DeviceId = Device->DeviceId;
	Result->Index = Device->Index;
	bResult = TRUE;
	return(bResult);
}
