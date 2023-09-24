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

CHAR 
PathRead(WIN_NAMEIDATA *Path)
{
	WCHAR *S = Path->S;
	WCHAR *R = Path->R;
	WCHAR C;

	while (C = *S++){
		if (C == ':'){		/* perl.exe */
			*R++ = '_';
		}else{
			*R++ = C;
		}
		if (*S == '/'){
			*S = '\\';
			break;
		}
	}
	*R = 0;
	Path->S = S;
	Path->R = R;
	return(*S);
}
BOOL 
PathGlobMount(WIN_NAMEIDATA *Path, DWORD Flags)
{
	LPWSTR pszDrive = win_basename(Path->Resolved);

	return(drive_lookup(MOUNTID(pszDrive[0]), Flags, Path));
}
BOOL 
PathGlobType(WIN_NAMEIDATA *Path, LPCWSTR TypeName)
{
	BOOL bResult = FALSE;
	CHAR *R;
	DWORD dwAttribs;

	win_wcscpy(Path->R, TypeName);
	dwAttribs = GetFileAttributesW(Path->Resolved);
	if (dwAttribs == -1){
		*Path->R = 0;		/* undo extension probe */
	}else{
		Path->Attribs = dwAttribs;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
PathGlobLink(WIN_NAMEIDATA *Path, WORD Depth)
{
	BOOL bResult = FALSE;

	if (vfs_readlink(Path, TRUE)){
//VfsDebugPath(Path, "vfs_lookup");
		if (!PathGlobType(Path, L".lnk")){
			bResult = TRUE;
		}else if (Depth >= WIN_SYMLOOP_MAX){
			SetLastError(ERROR_TOO_MANY_LINKS);
		}else{
			bResult = PathGlobLink(Path, Depth + 1);
		}
	}
	return(bResult);
}
BOOL 
PathGlob(WIN_NAMEIDATA *Path, DWORD Flags)
{
	BOOL bResult = TRUE;
	WIN32_FILE_ATTRIBUTE_DATA faData;
	Path->Attribs = GetFileAttributesW(Path->Resolved);

	if (Path->Attribs == FILE_ATTRIBUTE_MOUNT){
		bResult = PathGlobMount(Path, Flags);
	}else if (Path->Attribs != -1){
		return(TRUE);
	}else if (!PathGlobType(Path, L".lnk")){
		bResult = FALSE;
	}else if (Flags & WIN_FOLLOW){
		bResult = PathGlobLink(Path, 0);
	}else{
		Path->Attribs |= FILE_ATTRIBUTE_SYMLINK;
	}
//VfsDebugPath(Path, "PathGlob");
	return(bResult);
}
VOID 
PathOpen(WIN_NAMEIDATA *Path, LPWSTR Source)
{
	*Path->R = 0;
	Path->Attribs = -1;
	Path->S = Source;
}
VOID 
PathClose(WIN_NAMEIDATA *Path, DWORD Flags)
{
	DWORD dwFileType = WIN_VREG;

	if (PathGlob(Path, Flags)){
		if (Path->Attribs & FILE_ATTRIBUTE_DIRECTORY){
			dwFileType = WIN_VDIR;
		}else if (Path->Attribs & FILE_ATTRIBUTE_SYMLINK){
			dwFileType = WIN_VLNK;
		}
//		if (Path->Attribs == FILE_ATTRIBUTE_VFS){		/* perl.exe */
//VfsDebugPath(Path, "PathClose");
//		}
	}else if (PathGlobType(Path, L".exe")){
		Path->DeviceType = DEV_CLASS_DISK;
	}else if (PathGlobType(Path, L".vfs")){
		Path->FSType = FS_TYPE_DEVICE;
//	}else{
//		WIN_ERR("PathClose(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}
	Path->Flags = Flags;
	Path->FileType = dwFileType;
	Path->Last = Path->R - 1;
}

/****************************************************/

WIN_NAMEIDATA *
vfs_lookup(WIN_NAMEIDATA *Path, LPWSTR Source, DWORD Flags)
{
	PathOpen(Path, Source);
	while (PathRead(Path)){
		if (!PathGlob(Path, WIN_FOLLOW)){
//VfsDebugPath(Path, "vfs_lookup");
			Path->R = win_wcpcpy(Path->R, Path->S);
			Path->Last = Path->R - 1;
//			Path->FileType = 0;
			return(Path);
		}
	}
	PathClose(Path, Flags);
	return(Path);
}
BOOL 
vfs_namei(HANDLE Handle, DWORD Index, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	DWORD dwType = GetFileType(Handle);

	if (ERROR_SUCCESS != GetLastError()){
		WIN_ERR("GetFileType(%d): Index(%d) FileId(%d): %s\n", Handle, Index, Result->FileId, win_strerror(GetLastError()));
	}else switch (dwType){
		case FS_TYPE_DISK:
			bResult = disk_namei(Handle, Result);
			break;
		case FS_TYPE_CHAR:
			bResult = char_namei(Handle, Index, Result);
			break;
		case FS_TYPE_PIPE:
			bResult = pipe_namei(Handle, Result);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
//VfsDebugNode(Result, "vfs_namei");
	return(bResult);
}
