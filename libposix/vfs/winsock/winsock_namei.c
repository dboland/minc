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

/************************************************************/

BOOL 
WSAcceptFile(WIN_VNODE *Node, LPSOCKADDR Address, LPINT Length, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;
	SOCKET sResult;

	sResult = WSAAccept(Node->Socket, Address, Length, NULL, 0);
	if (sResult == INVALID_SOCKET){
		WIN_ERR("WSAAccept(%d): %s\n", Node->Socket, win_strerror(WSAGetLastError()));
	}else{
		Node->Pending = 0;
		Result->Socket = sResult;
		Result->FSType = Node->FSType;
		Result->DeviceType = Node->DeviceType;
		Result->FileType = Node->FileType;
		Result->Event = WSACreateEvent();
		Result->Size = WS2_SOCKET_SIZE;
		Result->Access = win_F_GETFL((HANDLE)sResult);;
		Result->Flags = win_F_GETFD((HANDLE)sResult);
		Result->CloseExec = FALSE;
		bResult = TRUE;
	}
	return(bResult);
}
