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
disk_F_DUPFD(WIN_VNODE *Node, HANDLE Process, DWORD Options, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	HANDLE hResult = NULL;
	BOOL bInherit = (Node->Flags & HANDLE_FLAG_INHERIT);

	if (!DuplicateHandle(GetCurrentProcess(), Node->Handle, Process, &hResult, 0, bInherit, Options)){
		WIN_ERR("disk_F_DUPFD(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else{
		Result->Handle = hResult;
		bResult = TRUE;
	}
	return(bResult);
}

/****************************************************/

BOOL 
disk_open(WIN_NAMEIDATA *Path, WIN_FLAGS *Flags, WIN_MODE *Mode, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;

	if (Flags->Access == GENERIC_WRITE){	/* git.exe (git-remote-https) */
		Flags->Access |= FILE_READ_ATTRIBUTES | FILE_READ_EA;
	}
	Flags->Share |= FILE_SHARE_DELETE;
	Flags->Access |= READ_CONTROL;		/* disk_fstat() */
	if (Path->FileType == WIN_VDIR){
		bResult = dir_open(Path, Flags, Mode, Result);
	}else{
		bResult = file_open(Path, Flags, Mode, Result);
	}
	return(bResult);
}
