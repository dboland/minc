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
vfs_readlink(WIN_NAMEIDATA *Path, BOOL MakeReal)
{
	BOOL bResult = FALSE;
	SHELL_LINK_HEADER slHead;
	DWORD dwSize = sizeof(SHELL_LINK_HEADER);
	CHAR szBuffer[MAX_PATH] = "";
	HANDLE hFile;

	hFile = CreateFileW(Path->Resolved, FILE_READ_DATA, FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
		Path->R = Path->Resolved;
		if (szBuffer[1] == ':'){	/* nano.exe */
			drive_lookup(MOUNTID(szBuffer[0]), WIN_NOCROSSMOUNT, Path);
		}else if (MakeReal){
			Path->R = win_basename(Path->Resolved);
		}
		Path->R = win_mbstowcp(Path->R, szBuffer, MAX_PATH);
		Path->Last = Path->R - 1;
		Path->Attribs = GetFileAttributesW(Path->Resolved);
		bResult = TRUE;
	}
	CloseHandle(hFile);
	return(bResult);
}
BOOL 
vfs_symlink(WIN_NAMEIDATA *Path, WIN_VATTR *Stat, WIN_MODE *Mode, WIN_NAMEIDATA *Result)
{
	BOOL bResult = FALSE;
	SHELL_LINK_HEADER slHead = {0};
	DWORD dwTerminalBlock = 0;
	DWORD dwSize;
	CHAR szBuffer[MAX_PATH] = "";
	HANDLE hFile;

	if (*Path->Last == '\\'){	/* GNU conftest.exe */
		*Path->Last = 0;
	}
	Result->R = win_wcpcpy(Result->R, L".lnk");
	hFile = CreateFileW(Result->Resolved, GENERIC_WRITE, FILE_SHARE_READ, NULL, 
		CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE){
		slHead.HeaderSize = sizeof(SHELL_LINK_HEADER);
		slHead.LinkCLSID = CLSID_ShellLink;
		slHead.LinkFlags = HasLinkInfo;
		slHead.FileAttributes = Stat->Attributes;
		slHead.CreationTime = Stat->CreationTime;
		slHead.AccessTime = Stat->LastAccessTime;
		slHead.WriteTime = Stat->LastWriteTime;
		slHead.FileSize = Stat->FileSizeLow;
		WriteFile(hFile, &slHead, sizeof(slHead), &dwSize, NULL);
		win_wcstombs(szBuffer, Path->Resolved, MAX_PATH);
		LinkCreateInfo(hFile, szBuffer, Stat);
		WriteFile(hFile, &dwTerminalBlock, sizeof(DWORD), &dwSize, NULL);
		bResult = CloseHandle(hFile);
	}
	return(bResult);
}
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
	 * 'anal-rententive memory management' (ONT, 2012),
	 * named pipes are not closed on program exit.
	 */
	while (dwIndex < WIN_OPEN_MAX){
		if (Nodes->FSType == FS_TYPE_PIPE && Nodes->FileType == WIN_VSOCK){
			pipe_close(Nodes);
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
			bResult = mail_read(Node->Handle, Buffer, Size, Result);
			break;
		case FS_TYPE_CHAR:
			bResult = char_read(Node, Buffer, Size, Result);
			break;
		case FS_TYPE_DEVICE:
			bResult = dev_read(Node->Device, Buffer, Size, Result);
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
			bResult = mail_write(Node->Handle, Buffer, Size, Result);
			break;
		case FS_TYPE_CHAR:
			bResult = char_write(Node, Buffer, Size, Result);
			break;
		case FS_TYPE_DEVICE:
			bResult = dev_write(Node->Device, Buffer, Size, Result);
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
		case FS_TYPE_DEVICE:
			bResult = dev_write(Node->Device, Buffer, Size, Result);
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
		case FS_TYPE_DEVICE:
			bResult = dev_read(Node->Device, Buffer, Size, Result);
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

	Result->FileId = Node->FileId;	/* dev_F_DUPFD() */
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

	if (!Node->Handle){
		SetLastError(ERROR_INVALID_HANDLE);
//	}else if (Node->Handle == Result->Handle){
//		bResult = TRUE;
	}else if (vfs_F_DUPFD(Node, FALSE, Result)){
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
	CloseHandle(hToken);
	LocalFree(psd);
	return(bResult);
}
BOOL 
vfs_chdir(WIN_TASK *Task, WIN_NAMEIDATA *Path)
{
	BOOL bResult = FALSE;

	if (*Path->Last != '\\'){
		*Path->R++ = '\\';
		*Path->R = 0;
	}
	if (Path->FileType != WIN_VDIR){
		SetLastError(ERROR_DIRECTORY);
	}else if (!vfs_access(Path, WIN_S_IRX)){
		return(FALSE);
	}else if (vfs_realpath(Path, WIN_PATH_MAX)){	/* building perl.exe */
		win_memcpy(&Task->Path, Path, sizeof(WIN_INODE));
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
	WIN_NAMEIDATA wPath;

	if (Node->Access & WRITE_DAC){
		bResult = disk_fchown(Node, NewUser, NewGroup);
	}else if (vfs_F_GETPATH(Node, &wPath)){		/* install.exe */
		bResult = disk_chown(&wPath, NewUser, NewGroup);
	}
	return(bResult);
}
BOOL 
vfs_rename(WIN_NAMEIDATA *Path, WIN_NAMEIDATA *Result)
{
	BOOL bResult = FALSE;

//VfsDebugPath(Path, "vfs_rename");
	switch (Path->FSType){
		case FS_TYPE_DISK:
			bResult = disk_rename(Path, Result);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
vfs_ftruncate(WIN_VNODE *Node, LONG Size)
{
	BOOL bResult = FALSE;

	if (-1 == SetFilePointer(Node->Handle, Size, NULL, FILE_BEGIN)){
		WIN_ERR("SetFilePointer(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else if (!SetEndOfFile(Node->Handle)){
		WIN_ERR("SetEndOfFile(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_truncate(WIN_NAMEIDATA *Path, WIN_FLAGS *Flags, LONG Size)
{
	BOOL bResult = FALSE;
	WIN_VNODE vNode = {0};
	WIN_MODE wMode = {0};

	if (!vfs_open(Path, Flags, &wMode, &vNode)){
		return(FALSE);
	}else if (vfs_ftruncate(&vNode, Size)){
		bResult = CloseHandle(vNode.Handle);
	}
	return(bResult);
}
BOOL 
vfs_link(WIN_NAMEIDATA *Path, WIN_NAMEIDATA *Result)
{
	BOOL bResult = FALSE;

	/* "Access is denied" if exists, a directory, or opened with write access */
	if (CreateHardLinkW(Result->Resolved, Path->Resolved, NULL)){
		bResult = TRUE;
//	}else{
//		WIN_ERR("CreateHardLink(%ls): %s\n", Result->Resolved, win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
vfs_unlink(WIN_NAMEIDATA *Path)
{
	BOOL bResult = FALSE;

//VfsDebugPath(Path, "vfs_unlink");
	switch (Path->FSType){
		case FS_TYPE_DEVICE:
		case FS_TYPE_DISK:
			bResult = disk_unlink(Path);
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
		case FS_TYPE_PIPE:	/* does work, but has weird results (perl.exe) */
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
		case FS_TYPE_DEVICE:
			bResult = dev_fsync(Node->Device);
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
//		win_strcpy(_WIN_ROOT, Path->Resolved);
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_revoke(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_DEVICE:
			bResult = dev_revoke(Node->Device);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
