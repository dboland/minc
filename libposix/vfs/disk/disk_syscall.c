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
	if (!win_acl_get_file(win_dirname(szDirName, Path->Resolved), &psd)){
		return(FALSE);
	}else if (!win_acl_init(Mode, &wControl)){
		WIN_ERR("win_acl_init(%s): %s\n", szDirName, win_strerror(GetLastError()));
	}else if (vfs_acl_create(psd, Mode, 0, &wControl)){
		hResult = CreateFileW(Path->Resolved, Flags->Access, Flags->Share, 
			&sa, Flags->Creation, Flags->Attribs, NULL);
		if (hResult != INVALID_HANDLE_VALUE){
			Result->Handle = hResult;
			Result->MountId = Path->MountId;
			Result->FSType = Path->FSType;
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
		Result->MountId = Path->MountId;
		Result->FSType = Path->FSType;
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
DiskOpenINode(WIN_NAMEIDATA *Path, DWORD Flags)
{
	BOOL bResult = FALSE;
	WIN_INODE iNode;
	DWORD dwResult;
	HANDLE hNode;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};

	hNode = CreateFileW(Path->Resolved, GENERIC_READ, FILE_SHARE_READ, 
		&sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hNode == INVALID_HANDLE_VALUE){
		WIN_ERR("CreateFile(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else if (!ReadFile(hNode, &iNode, sizeof(WIN_INODE), &dwResult, NULL)){
		WIN_ERR("ReadFile(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else if (iNode.Magic != TypeNameVirtual){
		SetLastError(ERROR_BAD_ARGUMENTS);
	}else{
		Path->DeviceId = iNode.DeviceId;
		Path->FileType = iNode.FileType;
		Path->FSType = iNode.FSType;
		Path->Size = iNode.NameSize;
		Path->Object = hNode;
		bResult = TRUE;
	}
	return(bResult);
}
