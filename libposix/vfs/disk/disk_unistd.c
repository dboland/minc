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
disk_close(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	switch (Node->FileType){
		case WIN_VDIR:
			bResult = dir_close(Node);
			break;
		default:
			bResult = file_close(Node);
	}
	return(bResult);
}
BOOL 
disk_fchown(WIN_VNODE *Node, PSID NewUser, PSID NewGroup)
{
	BOOL bResult = FALSE;
	PSECURITY_DESCRIPTOR psd;
	SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;
	FILETIME fTime = {0, 0};
	WIN_ACL_CONTROL wControl;

	if (!win_acl_get_fd(Node->Handle, &psd)){
		return(FALSE);
	}else if (!win_acl_dup(psd, &wControl)){
		WIN_ERR("win_acl_dup(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else if (vfs_acl_chown(psd, NewUser, NewGroup, &wControl)){
		GetSystemTimeAsFileTime(&fTime);
		if (!SetUserObjectSecurity(Node->Handle, &si, &wControl.Security)){
			WIN_ERR("SetUserObjectSecurity(%lu): %s\n", Node->Handle, win_strerror(GetLastError()));
		}else if (!SetFileTime(Node->Handle, &fTime, NULL, NULL)){
			WIN_ERR("SetFileTime(%lu): %s\n", fTime.dwHighDateTime, win_strerror(GetLastError()));
		}else{
			bResult = TRUE;
		}
	}
	LocalFree(psd);
	win_acl_free(&wControl);
	return(bResult);
}
BOOL 
disk_chown(WIN_NAMEIDATA *Path, PSID NewUser, PSID NewGroup)
{
	BOOL bResult = FALSE;
	WIN_FLAGS wFlags = {WRITE_DAC + FILE_WRITE_ATTRIBUTES, 
		FILE_SHARE_READ, OPEN_EXISTING, 0, 0};
	WIN_VNODE vNode = {0};
	WIN_MODE wMode = {0};

	if (!disk_open(Path, &wFlags, &wMode, &vNode)){
		return(FALSE);
	}else if (disk_fchown(&vNode, NewUser, NewGroup)){
		bResult = CloseHandle(vNode.Handle);
	}
	return(bResult);
}
BOOL 
disk_rmdir(WIN_NAMEIDATA *Path)
{
	BOOL bResult = FALSE;

	if (*Path->Last == '.'){		/* GNU conftest.exe */
		SetLastError(ERROR_BAD_ARGUMENTS);
	}else if (Path->FileType != WIN_VDIR){
		SetLastError(ERROR_DIRECTORY);
	}else{
		bResult = RemoveDirectoryW(Path->Resolved);
	}
	return(bResult);
}
BOOL 
disk_unlink(WIN_NAMEIDATA *Path)
{
	BOOL bResult = FALSE;

//vfs_ktrace("disk_unlink", STRUCT_NAMEI, Path);
	if (Path->FileType == WIN_VDIR){
		bResult = disk_rmdir(Path);
	}else if (*Path->Last == '\\'){		/* GNU conftest.exe */
		SetLastError(ERROR_BAD_PATHNAME);
	}else{
		bResult = DeleteFileW(Path->Resolved);
	}
	return(bResult);
}
BOOL 
disk_rename(WIN_NAMEIDATA *Path, WIN_NAMEIDATA *Result)
{
	BOOL bResult = FALSE;

	switch (Path->FileType){
		case WIN_VDIR:
			bResult = dir_rename(Path, Result);
			break;
		case WIN_VLNK:
			bResult = link_rename(Path, Result);
			break;
		case WIN_VREG:
			bResult = file_rename(Path, Result);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
disk_lseek(WIN_VNODE *Node, LARGE_INTEGER *Offset, DWORD Method, LARGE_INTEGER *Result)
{
	BOOL bResult = FALSE;

	if (SetFilePointerEx(Node->Handle, *Offset, Result, Method)){
		bResult = TRUE;
//	}else{
//		WIN_ERR("SetFilePointerEx(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
disk_pread(WIN_VNODE *Node, LPVOID Buffer, DWORD Size, DWORDLONG Offset, DWORD *Result)
{
	BOOL bResult = FALSE;
	OVERLAPPED ovl = {0, 0, Offset & 0xFFFFFFFF, Offset >> 32, NULL};
	LARGE_INTEGER liCurrent = {0};

	if (!SetFilePointerEx(Node->Handle, liCurrent, &liCurrent, FILE_CURRENT)){
		WIN_ERR("SetFilePointerEx(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else if (ReadFile(Node->Handle, Buffer, Size, Result, &ovl)){
		bResult = SetFilePointerEx(Node->Handle, liCurrent, &liCurrent, FILE_BEGIN);
	}else if (ERROR_HANDLE_EOF == GetLastError()){	/* vi.exe */
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
disk_pwrite(WIN_VNODE *Node, LPCVOID Buffer, DWORD Size, DWORDLONG Offset, DWORD *Result)
{
	BOOL bResult = FALSE;
	OVERLAPPED ovl = {0, 0, Offset & 0xFFFFFFFF, Offset >> 32, NULL};
	LARGE_INTEGER liCurrent = {0, 0};

	if (!SetFilePointerEx(Node->Handle, liCurrent, &liCurrent, FILE_CURRENT)){
		WIN_ERR("SetFilePointerEx(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else if (WriteFile(Node->Handle, Buffer, Size, Result, &ovl)){
		bResult = SetFilePointerEx(Node->Handle, liCurrent, &liCurrent, FILE_BEGIN);
	}
	return(bResult);
}
BOOL 
disk_readlink(WIN_NAMEIDATA *Path, BOOL MakeReal)
{
	BOOL bResult = FALSE;
	SHELL_LINK_HEADER slHead;
	DWORD dwSize = sizeof(SHELL_LINK_HEADER);
	CHAR szBuffer[MAX_PATH] = "";
	HANDLE hFile;
	LPWSTR pszBase = Path->Resolved;

	hFile = CreateFileW(Path->Resolved, FILE_READ_DATA, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE){
		return(FALSE);
	}else if (!ReadFile(hFile, &slHead, dwSize, &dwSize, NULL)){
		WIN_ERR("ReadFile(%s): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else if (!IsEqualGUID(&slHead.LinkCLSID, &CLSID_ShellLink)){
		SetLastError(ERROR_BAD_ARGUMENTS);
	}else{
		if (slHead.LinkFlags & HasLinkTargetIDList){
			LinkReadTarget(hFile);
		}
		if (slHead.LinkFlags & HasLinkInfo){
			LinkReadInfo(hFile, szBuffer);
		}
		if (szBuffer[1] == ':'){
			Path->MountId = MOUNTID(szBuffer[0]);	/* nano.exe */
		}else if (szBuffer[1] == '\\'){
			Path->MountId = 0;
		}else if (MakeReal){
			pszBase = Path->Base;
		}
		Path->R = win_mbstowcp(pszBase, szBuffer, MAX_PATH);
		Path->Last = Path->R - 1;
		Path->Attribs = slHead.FileAttributes;
//vfs_ktrace("disk_readlink", STRUCT_NAMEI, Path);
		bResult = TRUE;
	}
	CloseHandle(hFile);
	return(bResult);
}
BOOL 
disk_symlink(WIN_NAMEIDATA *Target, WIN_NAMEIDATA *Result)
{
	BOOL bResult = FALSE;
	DWORD dwTerminalBlock = 0;
	DWORD dwSize = sizeof(DWORD);
	HANDLE hFile;

	if (*Target->Last == '\\'){	/* GNU conftest.exe */
		*Target->Last = 0;
	}
	Result->R = win_wcpcpy(Result->R, L".lnk");
//vfs_ktrace("disk_symlink", STRUCT_NAMEI, Result);
	hFile = CreateFileW(Result->Resolved, GENERIC_WRITE, FILE_SHARE_READ, 
		NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE){
		LinkCreateFile(hFile, Target, Result->Resolved);
		WriteFile(hFile, &dwTerminalBlock, dwSize, &dwSize, NULL);
		bResult = CloseHandle(hFile);
	}
	return(bResult);
}
