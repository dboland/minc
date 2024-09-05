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

/************************************************************/

BOOL 
DiskGetEntity(WIN32_FIND_DATAW *Data, DWORD Index, WIN_DIRENT *Result)
{
	BOOL bResult = FALSE;
	LPWSTR pszType;

	if (!(Data->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)){
		Result->Index = Index;
		Result->FileType = WIN_VREG;
		win_wcscpy(Result->FileName, Data->cFileName);
		pszType = win_typename(Result->FileName);
		if (Data->dwFileAttributes == FILE_CLASS_INODE){
			Result->FileType = WIN_VLNK;
		}else if (Data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			Result->FileType = WIN_VDIR;
		}else if (!win_wcscmp(pszType, L".lnk")){
			Result->FileType = WIN_VLNK;
			*pszType = 0;			/* chop off extension */
		}
		bResult = TRUE;
	}
	return(bResult);
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
disk_getdents(WIN_NAMEIDATA *Path, WIN_DIRENT Entity[], DWORD Count, DWORD *Result)
{
	BOOL bResult = TRUE;
	LONG lResult = 0;
	WIN32_FIND_DATAW wfData;

//vfs_ktrace("disk_getdents", STRUCT_NAMEI, Path);
	if (Path->Index == -1){
//		bResult = disk_closedir(Node);
		bResult = FALSE;
	}else while (lResult < Count){
		if (!disk_readdir(Path, &wfData)){
			break;
		}else if (DiskGetEntity(&wfData, Path->Index, Entity)){
			Entity++;
			Path->Index++;
			lResult++;
		}
	}
	*Result = lResult;
	return(bResult);
}
