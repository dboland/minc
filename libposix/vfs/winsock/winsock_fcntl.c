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

#include <mswsock.h>

/****************************************************/

BOOL 
ws2_F_SETFL(WIN_VNODE *Node, WIN_FLAGS *Flags)
{
	BOOL bResult = FALSE;
	HANDLE hEvent = NULL;
	LONG lEvents = 0;

	/* Putting a WinSocket in non-blocking mode disables all connect() specific
	 * errors like WSAECONNREFUSED, WSAENETUNREACH, WSAETIMEDOUT, and turns
	 * them into the generic ERROR_UNEXP_NET_ERR with WriteFile().
	 */
	if (Flags->Attribs & FILE_FLAG_OVERLAPPED){	/* sshd.exe */
		hEvent = Node->Event;
		lEvents = FD_ALL_EVENTS;
	}
	if (SOCKET_ERROR == WSAEventSelect(Node->Socket, hEvent, lEvents)){
		WIN_ERR("WSAEventSelect(%d): %s\n", Node->Event, win_strerror(GetLastError()));
	}else{
		Node->Access = Flags->Access;
		Node->Attribs = Flags->Attribs;
		Node->CloseExec = Flags->CloseExec;
		bResult = TRUE;
	}
	return(bResult);
}
