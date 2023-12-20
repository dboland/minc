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
DiskGlobType(LPCWSTR TypeName, WIN_NAMEIDATA *Path)
{
	BOOL bResult = FALSE;
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
DiskGlobLink(LONG Depth, WIN_NAMEIDATA *Path)
{
	BOOL bResult = FALSE;

	if (disk_readlink(Path, TRUE)){
		if (!DiskGlobType(L".lnk", Path)){
			bResult = TRUE;
		}else if (Depth >= WIN_SYMLOOP_MAX){
			SetLastError(ERROR_TOO_MANY_LINKS);
		}else{
			bResult = DiskGlobLink(Depth + 1, Path);
		}
	}
	return(bResult);
}

/****************************************************/

BOOL 
disk_lookup(WIN_NAMEIDATA *Path, DWORD Flags)
{
	BOOL bResult = TRUE;

	if (!DiskGlobType(L".lnk", Path)){
		bResult = FALSE;
	}else if (Flags & WIN_FOLLOW){
		bResult = DiskGlobLink(1, Path);
	}else{
		Path->Attribs |= FILE_ATTRIBUTE_SYMLINK;
	}
	return(bResult);
}
BOOL 
disk_namei(HANDLE Handle, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	BY_HANDLE_FILE_INFORMATION fInfo;

	Result->Handle = Handle;
	Result->FSType = FS_TYPE_DISK;
	Result->DeviceType = DEV_CLASS_DISK;
	if (GetFileInformationByHandle(Handle, &fInfo)){
		if (fInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			Result->FileType = WIN_VDIR;
		}else{
			Result->FileType = WIN_VREG;
		}
		Result->Attribs = fInfo.dwFileAttributes;
		Result->Flags = win_F_GETFD(Handle);
		Result->Access = win_F_GETFL(Handle);
		bResult = TRUE;
	}else{
		WIN_ERR("GetFileInformationByHandle(%d): %s\n", Handle, win_strerror(GetLastError()));
	}
	return(bResult);
}
