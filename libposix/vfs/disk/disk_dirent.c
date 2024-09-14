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

static const UCHAR __DTYPE_POSIX[] = {
	WIN_DT_UNKNOWN,
	WIN_DT_REG,
	WIN_DT_DIR,
	WIN_DT_BLK,
	WIN_DT_CHR,
	WIN_DT_LNK,
	WIN_DT_SOCK,
	WIN_DT_FIFO,
	0
};

#define DIRENT_RECSIZE(namelen) \
    ((offsetof(WIN_DIRENT, FileName) + (namelen) + 1 + 7) &~ 7)

/************************************************************/

DWORD 
DiskGlobINode(WIN_NAMEIDATA *Path, LPCWSTR FileName)
{
	WIN_INODE iNode;
	HANDLE hResult;
	DWORD dwSize;
	DWORD dwResult = WIN_VREG;

	win_wcscpy(Path->Base, FileName);
	hResult = CreateFileW(Path->Resolved, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hResult == INVALID_HANDLE_VALUE){
		WIN_ERR("CreateFile(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else if (!ReadFile(hResult, &iNode, sizeof(WIN_INODE), &dwSize, NULL)){
		WIN_ERR("ReadFile(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else if (iNode.Magic == TypeNameVirtual){
		dwResult = iNode.FileType;
	}
	CloseHandle(hResult);
	return(dwResult);
}
DWORD 
DiskGetEntity(WIN32_FIND_DATAW *Data, WIN_NAMEIDATA *Path, PVOID Buffer)
{
	DWORD dwAttribs = Data->dwFileAttributes;
	WIN_DIRENT *pwdInfo = Buffer;
	DWORD dwFileType = WIN_VREG;
	LPWSTR pszType = win_typename(Data->cFileName);
	DWORD dwRecSize, dwNameSize;

	if (dwAttribs == FILE_CLASS_INODE){
		dwFileType = DiskGlobINode(Path, Data->cFileName);
	}else if (dwAttribs & FILE_ATTRIBUTE_DIRECTORY){
		dwFileType = WIN_VDIR;
	}else if (!win_wcscmp(pszType, L".lnk")){
		dwFileType = WIN_VLNK;
		*pszType = 0;			/* chop off extension */
	}
	dwNameSize = win_wcstombs(pwdInfo->FileName, Data->cFileName, WIN_MAXNAMLEN);
	dwRecSize = DIRENT_RECSIZE(dwNameSize);
	pwdInfo->FileId = (DWORDLONG)Path->Index;
	pwdInfo->Offset = (DWORDLONG)dwRecSize;
	pwdInfo->RecSize = dwRecSize;
	pwdInfo->NameSize = dwNameSize;
	pwdInfo->FileType = __DTYPE_POSIX[dwFileType];
	return(dwRecSize);
}

/****************************************************/

BOOL 
disk_closedir(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	if (!Node->Object){
		return(TRUE);
	}else if (!FindClose(Node->Object)){
		WIN_ERR("FindClose(0x%x): %s\n", Node->Object, win_strerror(GetLastError()));
	}else{
		Node->Object = NULL;
		Node->Index = 0;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
disk_rewinddir(WIN_NAMEIDATA *Path, WIN32_FIND_DATAW *Data)
{
	BOOL bResult = FALSE;
	HANDLE hResult = NULL;

	hResult = FindFirstFileW(Path->Resolved, Data);
	if (hResult == INVALID_HANDLE_VALUE){
		WIN_ERR("FindFirstFile(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else{
		Path->Object = hResult;
		Path->Index = 1;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
disk_readdir(WIN_NAMEIDATA *Path, WIN32_FIND_DATAW *Data)
{
	BOOL bResult = FALSE;

	if (!Path->Object){
		bResult = disk_rewinddir(Path, Data);
	}else if (FindNextFileW(Path->Object, Data)){
		bResult = TRUE;
	}else if (ERROR_NO_MORE_FILES == GetLastError()){
		Path->Index = -1;
	}else{
		WIN_ERR("FindNextFile(%d): %s\n", Path->Object, win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
disk_getdents(WIN_NAMEIDATA *Path, PVOID Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = TRUE;
	DWORD dwResult = 0;
	LONG lSize = Size;
	WIN32_FIND_DATAW wfData;
	DWORD dwSize;

//vfs_ktrace("disk_getdents", STRUCT_NAMEI, Path);
	if (Path->Index == -1){
		dwResult = 0;
	}else while (lSize >= sizeof(WIN_DIRENT)){
		if (!disk_readdir(Path, &wfData)){
			break;
		}else if (!(wfData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)){
			dwSize = DiskGetEntity(&wfData, Path, Buffer);
			dwResult += dwSize;
			lSize -= dwSize;
			Buffer += dwSize;
			Path->Index++;
		}
	}
	*Result = dwResult;
	return(bResult);
}
