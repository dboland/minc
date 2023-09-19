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

#include <sys/filio.h>

/****************************************************/

int 
file_FIONREAD(WIN_TASK *Task, WIN_VNODE *Node, int *nbytes)
{
	int result = -1;
	ULONG ulResult;

	if (!vfs_FIONREAD(Node, &ulResult)){
		__errno_posix(Task, GetLastError());
	}else{
		*nbytes = ulResult;
		result = 0;
	}
	return(result);
}
int 
file_FIONBIO(WIN_TASK *Task, WIN_VNODE *Node, int *mode)
{
	int result = -1;

	if (!vfs_FIONBIO(Node, *mode)){
		__errno_posix(Task, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}

/****************************************************/

int 
file_ioctl(WIN_TASK *Task, int fd, unsigned long request, va_list args)
{
	int result = 0;
	WIN_VNODE *pNode = &Task->Node[fd];

	switch (request){
		case FIONBIO:
			result = file_FIONBIO(Task, pNode, va_arg(args, int *));
			break;
		case FIONREAD:
			result = file_FIONREAD(Task, pNode, va_arg(args, int *));
			break;
		case FIOCLEX:	/* python.exe */
			pNode->CloseExec = TRUE;
			break;
		case FIONCLEX:
			pNode->CloseExec = FALSE;
			break;
		default:
			__errno_posix(Task, ERROR_NOT_SUPPORTED);
			result = -1;
	}
	return(result);
}
