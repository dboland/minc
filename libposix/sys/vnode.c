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

#define _KERNEL
#include <sys/fcntl.h>
#undef _KERNEL

/****************************************************/

HANDLE 
fd_win(WIN_TASK *Task, int fd)
{
	HANDLE hResult = INVALID_HANDLE_VALUE;

	if (fd == AT_FDCWD){
		return(INVALID_HANDLE_VALUE);
	}else if (fd < 0 || fd >= OPEN_MAX){
		SetLastError(ERROR_INVALID_HANDLE);
	}else{
		hResult = Task->Node[fd].Handle;
	}
	return(hResult);
}
int 
fd_posix(WIN_TASK *Task, WIN_VNODE *Node, int offset)
{
	int fd = offset > 0 ? offset : 0;
	WIN_VNODE *pNode = &Task->Node[fd];

	while (fd < OPEN_MAX){
		if (!pNode->Access){
			Node->FileId = fd;
			*pNode = *Node;
			return(fd);
		}
		pNode++;
		fd++;
	}
	__errno_posix(Task, ERROR_TOO_MANY_OPEN_FILES);
	return(-1);
}
