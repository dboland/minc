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

#include <shlobj.h>

/****************************************************/
BOOL 
shell_rename(WIN_NAMEIDATA *Path, WIN_NAMEIDATA *Result)
{
	BOOL bResult = FALSE;

	win_wcscpy(Result->R, L".lnk");
	if (Result->Attribs == -1){
		bResult = MoveFileW(Path->Resolved, Result->Resolved);
	}else if (Result->FileType != WIN_VLNK){
		SetLastError(ERROR_FILE_EXISTS);
	}else if (DeleteFileW(Result->Resolved)){
		bResult = MoveFileExW(Path->Resolved, Result->Resolved, MOVEFILE_COPY_ALLOWED);
	}
	return(bResult);
}
BOOL 
shell_unlink(WIN_NAMEIDATA *Path)
{
	return(DeleteFileW(Path->Resolved));
}
BOOL 
shell_readlink(WIN_NAMEIDATA *Path)
{
	BOOL bResult = FALSE;
	SHELL_LINK_HEADER slHead;
	WCHAR szBuffer[MAX_PATH];

	if (win_readlink(Path->Resolved, &slHead, szBuffer)){
		if (szBuffer[1] == ':'){
			Path->MountId = MOUNTID(szBuffer[0]);
		}
		Path->R = win_wcpcpy(Path->Resolved, szBuffer);
		Path->Last = Path->R - 1;
		Path->FSType = FS_TYPE_DISK;
		Path->Attribs = slHead.FileAttributes;
		bResult = TRUE;
	}
	return(bResult);
}
