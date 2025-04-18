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
pipe_close(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	if (Node->Handle == INVALID_HANDLE_VALUE){	/* socket not bound (init.exe) */
		bResult = TRUE;
	}else if (!CloseHandle(Node->Handle)){
		WIN_ERR("pipe_close(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else if (!SetEvent(Node->Event)){
		WIN_ERR("SetEvent(%d): %s\n", Node->Event, win_strerror(GetLastError()));
//	}else if (!CloseHandle(Node->Event)){
//		WIN_ERR("pipe_close(%d): %s\n", Node->Event, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	ZeroMemory(Node, sizeof(WIN_VNODE));
	return(bResult);
}
BOOL 
pipe_read(WIN_TASK *Task, WIN_VNODE *Node, LPSTR Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = FALSE;

	switch (Node->FileType){
		case WIN_VCHR:
			bResult = ReadFile(Node->Handle, Buffer, Size, Result, NULL);
			break;
		case WIN_VSOCK:
			bResult = sock_read(Task, Node, Buffer, Size, Result);
			break;
		case WIN_VFIFO:
			bResult = fifo_read(Node, Buffer, Size, Result);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
pipe_write(WIN_VNODE *Node, LPCSTR Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = FALSE;
	OVERLAPPED ovl = {0, 0, 0, 0, Node->Event};

	switch (Node->FileType){
		case WIN_VCHR:
			bResult = WriteFile(Node->Handle, Buffer, Size, Result, &ovl);
			break;
		case WIN_VSOCK:
			bResult = sock_write(Node, Buffer, Size, Result);
			break;
		case WIN_VFIFO:
			bResult = fifo_write(Node, Buffer, Size, Result);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
pipe_unlink(WIN_NAMEIDATA *Path)
{
	return(DeleteFileW(Path->Resolved));
}
