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

BOOL 
PdoOpenFile(WIN_NAMEIDATA *Path, WIN_FLAGS *Flags, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;

	Result->DeviceType = Path->DeviceType;
	Result->DeviceId = Path->DeviceId;
	Result->FileType = Path->FileType;
	Result->FSType = FS_TYPE_PDO;
	Result->Handle = Path->Handle;
	Result->Attribs = Flags->Attribs;
	Result->CloseExec = Flags->CloseExec;
	Result->Flags = win_F_GETFD(Path->Handle);
//	Result->Access = win_F_GETFL(Path->Handle);
	Result->Access = Flags->Access;
	MapGenericMask(&Result->Access, &AccessMap);
	Result->Device = Path->Device;
	return(TRUE);
}
BOOL 
PdoStatFile(HANDLE Handle, WIN_VATTR *Result)
{
	BOOL bResult = FALSE;
	PSECURITY_DESCRIPTOR psd;

	if (!win_acl_get_fd(Handle, &psd)){
		return(FALSE);
	}else if (!GetFileInformationByHandle(Handle, (BY_HANDLE_FILE_INFORMATION *)Result)){
		WIN_ERR("GetFileInformationByHandle(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else if (vfs_acl_stat(psd, Result)){
		Result->DeviceId = __Mounts->DeviceId;
		bResult = TRUE;
	}
	LocalFree(psd);
	return(bResult);
}

/****************************************************/

BOOL 
pdo_lookup(WIN_NAMEIDATA *Path, DWORD Flags)
{
	BOOL bResult = FALSE;
	WIN_INODE iNode;
	DWORD dwSize = sizeof(WIN_INODE);
	WIN_DEVICE *pwDevice;
	HANDLE hResult;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};

	hResult = CreateFileW(Path->Resolved, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, 
		&sa, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, NULL);
	if (hResult == INVALID_HANDLE_VALUE){
		WIN_ERR("pdo_lookup(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else if (!ReadFile(hResult, &iNode, dwSize, &dwSize, NULL)){
		WIN_ERR("ReadFile(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else if (iNode.Magic != TypeNameVirtual){
		SetLastError(ERROR_BAD_DEVICE);
	}else{
		pwDevice = DEVICE(iNode.DeviceId);
		Path->DeviceType = pwDevice->DeviceType;
		Path->DeviceId = iNode.DeviceId;
		Path->FileType = iNode.FileType;
		Path->FSType = FS_TYPE_PDO;
		Path->Attribs |= FILE_ATTRIBUTE_DEVICE;
		Path->Handle = hResult;
		Path->Device = pwDevice;
		bResult = TRUE;
//VfsDebugPath(Path, "pdo_lookup");
	}
	return(bResult);
}

