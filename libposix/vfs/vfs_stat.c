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
vfs_fstat(WIN_VNODE *Node, WIN_VATTR *Result)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_CHAR:
		case FS_TYPE_PDO:
			bResult = char_fstat(Node, Result);
			break;
		case FS_TYPE_PIPE:
			bResult = pipe_fstat(Node, Result);
			break;
		case FS_TYPE_MAILSLOT:
			bResult = mail_fstat(Node, Result);
			break;
		case FS_TYPE_WINSOCK:
			bResult = ws2_fstat(Node, Result);
			break;
		default:
			bResult = disk_fstat(Node, Result);
	}
	return(bResult);
}
BOOL 
vfs_stat(WIN_NAMEIDATA *Path, WIN_VATTR *Result)
{
	BOOL bResult = FALSE;

	/* CreateFile() yields "Access is denied" on open files
	 * which previously have been deleted (git.exe).
	 */
	if (Path->Attribs == -1){
		return(FALSE);
	}else switch (Path->FSType){
		case FS_TYPE_DISK:
			bResult = disk_stat(Path, Result);
			break;
		case FS_TYPE_PDO:
			bResult = pdo_stat(Path, Result);
			break;
		case FS_TYPE_SHELL:
			bResult = shell_stat(Path, Result);
			break;
		case FS_TYPE_PIPE:
			bResult = pipe_stat(Path, Result);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
vfs_chmod(WIN_NAMEIDATA *Path, WIN_MODE *Mode)
{
	BOOL bResult = FALSE;

	switch (Path->FSType){
		case FS_TYPE_PIPE:
		case FS_TYPE_PDO:
		case FS_TYPE_DISK:
			bResult = disk_chmod(Path, Mode);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
vfs_fchmod(WIN_VNODE *Node, WIN_MODE *Mode)
{
	BOOL bResult = FALSE;
	WIN_NAMEIDATA wPath = {0};

	if (Node->Access & WRITE_DAC){
		bResult = disk_fchmod(Node, Mode);
	}else if (vfs_F_GETPATH(Node, &wPath)){		/* install.exe */
		bResult = vfs_chmod(&wPath, Mode);
	}
	return(bResult);
}
BOOL 
vfs_mknod(WIN_NAMEIDATA *Path, WIN_MODE *Mode, DWORD DeviceId)
{
	BOOL bResult = FALSE;
	WIN_INODE iNode = {TypeNameVirtual, DeviceId, Mode->FileType, 
		FS_TYPE_PDO, 0, 0};
	DWORD dwResult;
	HANDLE hNode;

	hNode = CreateFileW(Path->Resolved, GENERIC_WRITE, FILE_SHARE_READ, 
		NULL, CREATE_ALWAYS, FILE_CLASS_INODE, NULL);
	if (hNode == INVALID_HANDLE_VALUE){
		return(FALSE);
	}else if (!WriteFile(hNode, &iNode, sizeof(WIN_INODE), &dwResult, NULL)){
		WIN_ERR("WriteFile(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else{
		bResult = CloseHandle(hNode);
	}
	return(bResult);
}
BOOL 
vfs_mkfifo(WIN_NAMEIDATA *Path, WIN_MODE *Mode, WIN_VNODE *Result)
{
	HANDLE hResult;
	BOOL bResult = FALSE;

	hResult = CreateNamedPipeW(Path->Resolved, PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE, 
		PIPE_UNLIMITED_INSTANCES, WIN_PIPE_BUF, WIN_PIPE_BUF, NMPWAIT_USE_DEFAULT_WAIT, NULL);
	if (hResult == INVALID_HANDLE_VALUE){
		WIN_ERR("CreateNamedPipe(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else{
		Result->Handle = hResult;
		Result->FSType = FS_TYPE_PIPE;
		Result->FileType = WIN_VFIFO;
		Result->Access = GENERIC_READ | GENERIC_WRITE;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_mkdir(WIN_NAMEIDATA *Path, WIN_MODE *Mode)
{
	BOOL bResult = FALSE;
	DWORD dwSize = 0;
	PSECURITY_DESCRIPTOR psd = NULL;
	WCHAR szParent[WIN_PATH_MAX];
	WIN_ACL_CONTROL wControl;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), &wControl.Security, FALSE};
	BYTE bAceFlags = OBJECT_INHERIT_ACE + CONTAINER_INHERIT_ACE;

	if (*Path->Last == '\\'){	/* git.exe */
		*Path->Last = 0;
	}
	if (!win_acl_get_file(win_dirname(szParent, Path->Resolved), &psd)){
		return(FALSE);
	}else if (!win_acl_init(Mode, &wControl)){
		WIN_ERR("win_acl_init(%s): %s\n", szParent, win_strerror(GetLastError()));
	}else if (vfs_acl_create(psd, Mode, bAceFlags, &wControl)){
		bResult = CreateDirectoryW(Path->Resolved, &sa);
	}
	LocalFree(psd);
	win_acl_free(&wControl);
	return(bResult);
}
