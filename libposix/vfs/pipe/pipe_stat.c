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
pipe_fstat(WIN_VNODE *Node, WIN_VATTR *Result)
{
	BOOL bResult = FALSE;
	PSECURITY_DESCRIPTOR psd;
	FILETIME ftNow;

	if (!win_acl_get_fd(Node->Handle, &psd)){
		return(FALSE);
	}else if (vfs_acl_stat(psd, Result)){
		GetSystemTimeAsFileTime(&ftNow);
		Result->CreationTime = ftNow;
		Result->LastAccessTime = ftNow;
		Result->LastWriteTime = ftNow;
		Result->NumberOfLinks = 1;
		Result->DeviceId = __Mounts->DeviceId;
		Result->SpecialId = Node->DeviceId;
		Result->Mode.FileType = Node->FileType;
		bResult = TRUE;
	}
	LocalFree(psd);
	return(bResult);
}
BOOL 
pipe_mknod(LPWSTR FileName, LPWSTR NtName, WIN_MODE *Mode)
{
	BOOL bResult = FALSE;
	DWORD dwSize = sizeof(WIN_INODE);
	WIN_INODE iNode = {TypeNameVirtual, Mode->FileType, DEV_CLASS_CPU, 0, FS_TYPE_PIPE, 0};
	HANDLE hResult;

	win_wcscpy(iNode.NtName, NtName);
__PRINTF("  pipe_mknod(%ls): %ls\n", FileName, NtName)
	hResult = CreateFileW(FileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, 
		CREATE_ALWAYS, FILE_ATTRIBUTE_SYSTEM, NULL);
	if (hResult == INVALID_HANDLE_VALUE){
		WIN_ERR("CreateFile(%ls): %s\n", FileName, win_strerror(GetLastError()));
	}else if (!WriteFile(hResult, &iNode, dwSize, &dwSize, NULL)){
		WIN_ERR("WriteFile(%ls): %s\n", FileName, win_strerror(GetLastError()));
	}else{
		bResult = CloseHandle(hResult);
	}
	return(bResult);
}
