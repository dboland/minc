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
SHGlobType(LPCWSTR TypeName, WIN_NAMEIDATA *Path)
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
SHGlobLink(WIN_NAMEIDATA *Path, LONG Depth)
{
	BOOL bResult = FALSE;

	/* Return invalid Shell links as regular files.
	 */
	if (shell_F_LOOKUP(Path, WIN_ISSYMLINK)){
		if (!SHGlobType(L".lnk", Path)){
			bResult = TRUE;
		}else if (Depth >= WIN_SYMLOOP_MAX){
			SetLastError(ERROR_TOO_MANY_LINKS);
		}else{
			bResult = SHGlobLink(Path, Depth + 1);
		}
	}else{
		bResult = TRUE;
//		vfs_ktrace("SHGlobLink", STRUCT_NAMEI, Path);
	}
	return(bResult);
}

/****************************************************/

BOOL 
shell_lookup(WIN_NAMEIDATA *Path, DWORD Flags)
{
	BOOL bResult = TRUE;

	if (!SHGlobType(L".lnk", Path)){
		bResult = FALSE;
	}else if (Flags & WIN_FOLLOW){
		bResult = SHGlobLink(Path, 0);
	}else{
		Path->FSType = FS_TYPE_SHELL;
		Path->Attribs |= FILE_ATTRIBUTE_SYMLINK;
	}
	return(bResult);
}
