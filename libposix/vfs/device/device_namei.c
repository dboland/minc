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

GENERIC_MAPPING AccessMap = {WIN_S_IREAD, WIN_S_IWRITE, WIN_S_IEXEC, WIN_S_IRWX};

/****************************************************/

HANDLE 
DevOpenChar(LPCSTR Name, WIN_FLAGS *Flags, PSECURITY_ATTRIBUTES sa)
{
	HANDLE hResult = NULL;

	hResult = CreateFile(Name, Flags->Access, Flags->Share, sa, 
		Flags->Creation, Flags->Attribs, NULL);
	if (hResult == INVALID_HANDLE_VALUE){
		WIN_ERR("CreateFile(%s): %s\n", Name, win_strerror(GetLastError()));
	}
	return(hResult);
}
BOOL 
DevOpenFile(WIN_NAMEIDATA *Path, WIN_FLAGS *Flags, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	HANDLE hResult;
	WIN_INODE iNode;
	DWORD dwSize = sizeof(WIN_INODE);
	WIN_DEVICE *pwDevice;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};

	hResult = CreateFileW(Path->Resolved, GENERIC_READ, FILE_SHARE_READ, &sa, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hResult == INVALID_HANDLE_VALUE){
		return(FALSE);
	}else if (!ReadFile(hResult, &iNode, dwSize, &dwSize, NULL)){
		WIN_ERR("DevOpenFile(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else if (iNode.Magic != TypeNameVirtual){
		SetLastError(ERROR_BAD_DEVICE);
	}else{
		pwDevice = DEVICE(iNode.DeviceId);
		Result->Handle = hResult;
		Result->FSType = FS_TYPE_DEVICE;
		Result->FileType = iNode.FileType;
		Result->DeviceType = pwDevice->DeviceType;
		Result->DeviceId = iNode.DeviceId;
		Result->Attribs = Flags->Attribs;
		Result->CloseExec = Flags->CloseExec;
		Result->Flags = win_F_GETFD(hResult);
//		Result->Access = win_F_GETFL(hResult);
		Result->Access = Flags->Access;
		MapGenericMask(&Result->Access, &AccessMap);
		Result->Device = pwDevice;
		bResult = TRUE;
//VfsDebugDevice(pDevice, "DevOpenFile");
	}
	return(bResult);
}
