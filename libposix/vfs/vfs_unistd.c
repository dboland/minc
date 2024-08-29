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
vfs_close(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;
	DWORD dwFileId = Node->FileId;

	if (!Node->Handle){		/* vfs_dup2() */
		bResult = TRUE;
	}else switch (Node->FSType){
		case FS_TYPE_PIPE:
			bResult = pipe_close(Node);
			break;
		case FS_TYPE_WINSOCK:
			bResult = ws2_close(Node);
			break;
		case FS_TYPE_PDO:
			bResult = pdo_close(Node);
			break;
		default:
			bResult = disk_close(Node);
	}
	Node->FileId = dwFileId;
	return(bResult);
}
VOID 
vfs_closefrom(WIN_VNODE Nodes[])
{
	DWORD dwIndex = 0;

	/* Contrary to Raymond Chen's rant on
	 * 'anal-rententive memory management' (The Old New Thing, 2012),
	 * named pipes are not closed on program exit (login.exe).
	 */
	while (dwIndex < WIN_OPEN_MAX){
		if (Nodes->FileType == WIN_VSOCK){
			vfs_close(Nodes);
		}
		dwIndex++;
		Nodes++;
	}
}
BOOL 
vfs_read(WIN_VNODE *Node, LPVOID Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_DISK:
			bResult = ReadFile(Node->Handle, Buffer, Size, Result, NULL);
			break;
		case FS_TYPE_WINSOCK:
			bResult = ws2_read(Node, Buffer, Size, Result);
			break;
		case FS_TYPE_PIPE:
			bResult = pipe_read(Node, Buffer, Size, Result);
			break;
		case FS_TYPE_MAILSLOT:
			bResult = mail_read(Node, Buffer, Size, Result);
			break;
		case FS_TYPE_CHAR:
			bResult = char_read(Node, Buffer, Size, Result);
			break;
		case FS_TYPE_PDO:
			bResult = pdo_read(DEVICE(Node->DeviceId), Buffer, Size, Result);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
vfs_write(WIN_VNODE *Node, LPCVOID Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_DISK:
			bResult = WriteFile(Node->Handle, Buffer, Size, Result, NULL);
			break;
		case FS_TYPE_WINSOCK:
			bResult = ws2_write(Node, Buffer, Size, Result);
			break;
		case FS_TYPE_PIPE:
			bResult = pipe_write(Node, Buffer, Size, Result);
			break;
		case FS_TYPE_MAILSLOT:
			bResult = mail_write(Node, Buffer, Size, Result);
			break;
		case FS_TYPE_CHAR:
			bResult = char_write(Node, Buffer, Size, Result);
			break;
		case FS_TYPE_PDO:
			bResult = pdo_write(DEVICE(Node->DeviceId), Buffer, Size, Result);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
vfs_pwrite(WIN_VNODE *Node, LPCVOID Buffer, DWORD Size, DWORDLONG Offset, DWORD *Result)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_DISK:
			bResult = disk_pwrite(Node, Buffer, Size, Offset, Result);
			break;
		case FS_TYPE_PIPE:
		case FS_TYPE_WINSOCK:
			SetLastError(ERROR_PIPE_CONNECTED);
			break;
		case FS_TYPE_PDO:
			bResult = pdo_write(DEVICE(Node->DeviceId), Buffer, Size, Result);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
vfs_pread(WIN_VNODE *Node, LPVOID Buffer, DWORD Size, DWORDLONG Offset, DWORD *Result)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_DISK:		/* git.exe (clone-https) */
			bResult = disk_pread(Node, Buffer, Size, Offset, Result);
			break;
		case FS_TYPE_PIPE:
		case FS_TYPE_WINSOCK:
			SetLastError(ERROR_PIPE_CONNECTED);
			break;
		case FS_TYPE_PDO:
			bResult = pdo_read(DEVICE(Node->DeviceId), Buffer, Size, Result);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
vfs_dup(WIN_VNODE *Node, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;

	Result->FileId = Node->FileId;		/* pdo_F_DUPFD() */
	if (vfs_F_DUPFD(Node, FALSE, Result)){
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_dup2(WIN_VNODE *Node, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	WIN_VNODE vnCurrent = *Result;

	if (vfs_F_DUPFD(Node, FALSE, Result)){
		bResult = vfs_close(&vnCurrent);
	}
	return(bResult);
}
BOOL 
vfs_access(WIN_NAMEIDATA *Path, ACCESS_MASK Access)
{
	BOOL bResult = FALSE;
	HANDLE hToken = NULL;
	PSECURITY_DESCRIPTOR psd;
	GENERIC_MAPPING map = {WIN_S_IREAD, WIN_S_IWRITE, WIN_S_IEXEC, WIN_S_IRWX};
	PRIVILEGE_SET priv = {0};
	ACCESS_MASK amGranted;
	DWORD dwSize = sizeof(PRIVILEGE_SET);

	if (!Access){
		return(Path->Attribs != -1);
	}else if (!win_acl_get_file(Path->Resolved, &psd)){
		return(FALSE);
	}else if (!win_cap_get_proc(MAXIMUM_ALLOWED, TokenImpersonation, &hToken)){
		WIN_ERR("win_cap_get_proc(%d): %s\n", GetCurrentProcess(), win_strerror(GetLastError()));
	}else if (!AccessCheck(psd, hToken, Access, &map, &priv, &dwSize, &amGranted, &bResult)){
		WIN_ERR("AccessCheck(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}
//vfs_ktrace("amGranted", STRUCT_ACCESS, &amGranted);
	CloseHandle(hToken);
	LocalFree(psd);
	return(bResult);
}
BOOL 
vfs_chdir(WIN_TASK *Task, WIN_NAMEIDATA *Path)
{
	BOOL bResult = FALSE;

	/* perl.exe creates really weird paths.
	 */
	if (Path->FileType != WIN_VDIR){
		SetLastError(ERROR_DIRECTORY);
	}else if (!vfs_access(Path, WIN_S_IEXEC)){	/* network drives (Synology) */
		return(FALSE);
	}else if (win_realpath(Path->Resolved, WIN_PATH_MAX, __Strings[Task->TaskId].Path)){
		win_wcscpy(__Strings[Task->TaskId].Path, Path->Resolved);
		Task->MountId = Path->MountId;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_chown(WIN_NAMEIDATA *Path, PSID NewUser, PSID NewGroup)
{
	return(disk_chown(Path, NewUser, NewGroup));
}
BOOL 
vfs_fchown(WIN_VNODE *Node, PSID NewUser, PSID NewGroup)
{
	BOOL bResult = FALSE;
	WIN_NAMEIDATA wPath = {0};

	if (Node->Access & WRITE_DAC){
		bResult = disk_fchown(Node, NewUser, NewGroup);
	}else if (vfs_F_GETPATH(Node, &wPath)){		/* install.exe */
		bResult = disk_chown(&wPath, NewUser, NewGroup);
	}
	return(bResult);
}
BOOL 
vfs_ftruncate(WIN_VNODE *Node, LARGE_INTEGER *Size)
{
	BOOL bResult = FALSE;

	if (-1 == SetFilePointerEx(Node->Handle, *Size, Size, FILE_BEGIN)){
		WIN_ERR("SetFilePointerEx(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else if (!SetEndOfFile(Node->Handle)){
		WIN_ERR("SetEndOfFile(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_truncate(WIN_NAMEIDATA *Path, LARGE_INTEGER *Size)
{
	BOOL bResult = FALSE;
	WIN_VNODE vNode = {0};
	WIN_FLAGS wFlags = {GENERIC_WRITE, FILE_SHARE_READ, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, FALSE};
	WIN_MODE wMode = {0};

	if (!disk_open(Path, &wFlags, &wMode, &vNode)){
		return(FALSE);
	}else if (vfs_ftruncate(&vNode, Size)){
		bResult = CloseHandle(vNode.Handle);
	}
	return(bResult);
}
BOOL 
vfs_link(WIN_NAMEIDATA *Path, WIN_NAMEIDATA *Target)
{
	BOOL bResult = FALSE;

	/* CreateHardLink() yields "Access is denied" if target exists,
	 * source is a directory, opened for writing or does not exist.
	 */
	if (Path->Attribs == -1){
		return(FALSE);
	}else if (Target->Attribs != -1){
		SetLastError(ERROR_FILE_EXISTS);
	}else if (CreateHardLinkW(Target->Resolved, Path->Resolved, NULL)){
		bResult = TRUE;
//	}else{
//		WIN_ERR("CreateHardLink(%ls): %s\n", Target->Resolved, win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
vfs_unlink(WIN_NAMEIDATA *Path)
{
	BOOL bResult = FALSE;

	switch (Path->FSType){
		case FS_TYPE_DISK:
			bResult = disk_unlink(Path);
			break;
		case FS_TYPE_PDO:
			bResult = pdo_unlink(Path);
			break;
		case FS_TYPE_SHELL:
			bResult = shell_unlink(Path);
			break;
		case FS_TYPE_PIPE:
			bResult = pipe_unlink(Path);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
vfs_rename(WIN_NAMEIDATA *Path, WIN_NAMEIDATA *Result)
{
	BOOL bResult = FALSE;

	switch (Path->FSType){
		case FS_TYPE_DISK:
			bResult = disk_rename(Path, Result);
			break;
		case FS_TYPE_SHELL:
			bResult = shell_rename(Path, Result);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
vfs_rmdir(WIN_NAMEIDATA *Path)
{
	BOOL bResult = FALSE;

	switch (Path->FSType){
		case FS_TYPE_DISK:
			bResult = disk_rmdir(Path);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
vfs_lseek(WIN_VNODE *Node, LARGE_INTEGER *Offset, DWORD Method, LARGE_INTEGER *Result)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_CHAR:
			SetLastError(ERROR_INVALID_HANDLE);
			break;
		case FS_TYPE_PIPE:
		case FS_TYPE_WINSOCK:
			SetLastError(ERROR_PIPE_CONNECTED);
			break;
		default:
			bResult = disk_lseek(Node, Offset, Method, Result);
	}
	return(bResult);
}
BOOL 
vfs_sync(VOID)
{
	/* umount.exe */
	return(TRUE);
}
BOOL 
vfs_fsync(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_DISK:
			bResult = FlushFileBuffers(Node->Handle);
			break;
		case FS_TYPE_PDO:
			bResult = pdo_fsync(DEVICE(Node->DeviceId));
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
vfs_chroot(WIN_NAMEIDATA *Path)
{
	BOOL bResult = FALSE;

	if (Path->FileType != WIN_VDIR){
		SetLastError(ERROR_DIRECTORY);
	}else if (!win_group_member(&SidAdmins)){
		SetLastError(ERROR_PRIVILEGE_NOT_HELD);
	}else{
//		bResult = win_chroot(Path->Resolved);
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_revoke(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_PDO:
			bResult = pdo_revoke(DEVICE(Node->DeviceId));
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
vfs_symlink(WIN_NAMEIDATA *Path, WIN_NAMEIDATA *Target)
{
	BOOL bResult = FALSE;
	DWORD dwResult;
	WIN_INODE iNode = {TypeNameVirtual, Path->DeviceId, WIN_VLNK, 
		FS_TYPE_DISK, INAMESIZE(Target->Resolved), 0};
	HANDLE hNode;

//vfs_ktrace("vfs_symlink", STRUCT_NAMEI, Target);
	hNode = CreateFileW(Path->Resolved, GENERIC_WRITE, FILE_SHARE_READ, 
		NULL, CREATE_NEW, FILE_CLASS_INODE, NULL);
	if (hNode == INVALID_HANDLE_VALUE){
		return(FALSE);
	}else if (!WriteFile(hNode, &iNode, sizeof(WIN_INODE), &dwResult, NULL)){
		WIN_ERR("WriteFile(%d): %s\n", hNode, win_strerror(GetLastError()));
	}else if (WriteFile(hNode, Target->Resolved, iNode.NameSize, &dwResult, NULL)){
		bResult = CloseHandle(hNode);
	}
	return(bResult);
}
BOOL 
vfs_readlink(WIN_NAMEIDATA *Path)
{
	BOOL bResult = FALSE;

	switch (Path->FSType){
		case FS_TYPE_DISK:
			bResult = disk_readlink(Path);
			break;
		case FS_TYPE_SHELL:
			bResult = shell_readlink(Path);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
