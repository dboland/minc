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
DiskCreateFile(WIN_NAMEIDATA *Path, WIN_FLAGS *Flags, WIN_MODE *Mode, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	PSECURITY_DESCRIPTOR psd;
	WCHAR szDirName[WIN_PATH_MAX] = L"";
	HANDLE hResult;
	WIN_ACL_CONTROL wControl;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), &wControl.Security, FALSE};

	Flags->Access |= WRITE_DAC | WRITE_OWNER;		/* disk_fchown() */
	if (!win_acl_get_file(win_dirname_r(Path->Resolved, szDirName), &psd)){
		return(FALSE);
	}else if (!win_acl_init(Mode, &wControl)){
		WIN_ERR("win_acl_init(%s): %s\n", szDirName, win_strerror(GetLastError()));
	}else if (vfs_acl_create(psd, Mode, 0, &wControl)){
		hResult = CreateFileW(Path->Resolved, Flags->Access, Flags->Share, 
			&sa, Flags->Creation, Flags->Attribs, NULL);
		if (hResult != INVALID_HANDLE_VALUE){
			Result->Handle = hResult;
			Result->FSType = Path->FSType;
			Result->DeviceType = Path->DeviceType;
			Result->DeviceId = Path->DeviceId;
			Result->FileType = Path->FileType;
			Result->Attribs = Flags->Attribs;
			Result->CloseExec = Flags->CloseExec;
			Result->Access = win_F_GETFL(hResult);
			Result->Flags = win_F_GETFD(hResult);
			bResult = TRUE;
		}
	}
	LocalFree(psd);
	win_acl_free(&wControl);
	return(bResult);
}
BOOL 
DiskOpenFile(WIN_NAMEIDATA *Path, WIN_FLAGS *Flags, WIN_VNODE *Result)
{
	HANDLE hResult;
	BOOL bResult = FALSE;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, FALSE};

	hResult = CreateFileW(Path->Resolved, Flags->Access, Flags->Share, 
		&sa, Flags->Creation, Flags->Attribs, NULL);
	if (hResult != INVALID_HANDLE_VALUE){
		Result->Handle = hResult;
		Result->FSType = Path->FSType;
		Result->DeviceType = Path->DeviceType;
		Result->DeviceId = Path->DeviceId;
		Result->FileType = Path->FileType;
		Result->Attribs = Flags->Attribs;
		Result->CloseExec = Flags->CloseExec;
		Result->Access = win_F_GETFL(hResult);
		Result->Flags = win_F_GETFD(hResult);
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
DiskStatFile(LPCWSTR FileName, DWORD Attribs, WIN_VATTR *Result)
{
	BOOL bResult = FALSE;
	PSECURITY_DESCRIPTOR psd;
	HANDLE hFile;

	hFile = CreateFileW(FileName, READ_CONTROL, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, Attribs, NULL);
	if (hFile == INVALID_HANDLE_VALUE){
		return(FALSE);
	}else if (!win_acl_get_fd(hFile, &psd)){
		return(FALSE);
	}else if (!GetFileInformationByHandle(hFile, (BY_HANDLE_FILE_INFORMATION *)Result)){
		WIN_ERR("GetFileInformationByHandle(%d): %s\n", hFile, win_strerror(GetLastError()));
	}else if (vfs_acl_stat(psd, Result)){
		bResult = CloseHandle(hFile);
	}
	LocalFree(psd);
	return(bResult);
}
