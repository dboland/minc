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

SHORT 
SockPollError(WIN_VNODE *Node, DWORD Error)
{
	SHORT sResult = WIN_POLLERR;
	OVERLAPPED ovl = {0, 0, 0, 0, Node->Event};

	switch (Error){
		case ERROR_BROKEN_PIPE:		/* 109: The pipe has been ended (write end closed) */
			DisconnectNamedPipe(Node->Handle);
			/* Segfaults with OVERLAPPED struct,
			 * and completion proc missing.
			 */
			ConnectNamedPipe(Node->Handle, &ovl);
		case ERROR_BAD_PIPE:		/* 230: The pipe state is invalid */
			/* This error indicates we need to ConnectNamedPipe().
			 * In OVERLAPPED mode, it loses its meaning.
			 */
			sResult = 0;
			break;
		default:
			WIN_ERR("SockPollError(%d): %s\n", Node->Handle, win_strerror(Error));
	}
	return(sResult);
}

/****************************************************/

DWORD 
sock_poll(WIN_VNODE *Node, WIN_POLLFD *Info)
{
	SHORT sResult = WIN_POLLOUT;
	SHORT sMask = Info->Events | WIN_POLLIGNORE;
	DWORD dwAvail = 0;
	DWORD dwMessage = 0;
	DWORD dwResult = 0;

	if (!PeekNamedPipe(Node->Handle, NULL, 0, NULL, &dwAvail, &dwMessage)){
		sResult = SockPollError(Node, GetLastError());
	}else if (dwAvail || dwMessage){
		sResult = WIN_POLLIN;
	}
	if (Info->Result = sResult & sMask){
		dwResult++;
	}
	return(dwResult);
}
