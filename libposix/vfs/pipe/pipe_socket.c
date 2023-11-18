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

VOID 
SockError(HRESULT Error)
{
	INT wsaError = 0;

	/* /c/MinGW/include/winsock2.h */
	switch (Error){
		case ERROR_FILE_NOT_FOUND:
			wsaError = WSAENOENT;
			break;
		case ERROR_INVALID_PARAMETER:
		case ERROR_INVALID_NAME:
			wsaError = WSAEINVAL;
			break;
		case ERROR_INVALID_HANDLE:
		case ERROR_PATH_NOT_FOUND:
			wsaError = WSAENOTSOCK;
			break;
		case ERROR_PIPE_BUSY:			/* All pipe instances are busy */
			wsaError = WSAEISCONN;		/* ./lib/libc/gen/syslog_r.c */
			SetEvent(__PipeEvent);		/* logger.exe */
			break;
		case ERROR_PIPE_NOT_CONNECTED:	/* No process is on the other end of the pipe */
			wsaError = WSAENOTCONN;
			break;
		case ERROR_NO_DATA:			// 232: The pipe is being closed (read end closed)
			wsaError = ERROR_NO_DATA;
			break;
		case ERROR_BROKEN_PIPE:
			wsaError = WSAEPIPE;
			break;
		case ERROR_SIGNAL_PENDING:
			wsaError = WSAEINTR;
			break;
		case ERROR_NOACCESS:
			wsaError = WSAEFAULT;
			break;
		case ERROR_ACCESS_DENIED:
			wsaError = WSAEACCES;
			break;
//		case ERROR_PIPE_CONNECTED:		/* There is a process on other end of the pipe */
//		case ERROR_PIPE_LISTENING:		/* ReadFile() after ConnectNamedPipe() */
		default:
			WIN_ERR("SockError(%d): %s\n", Error, win_strerror(Error));
	}
	WSASetLastError(wsaError);
}
PVOID 
SockAllocData(WSAMSG *Message, DWORD *Size, DWORD *Result)
{
	DWORD dwSize = 0;
	DWORD dwIndex = 0;
	DWORD dwCount = Message->dwBufferCount;
	LPWSABUF pwsData = Message->lpBuffers;

	while (dwIndex < dwCount){
		dwSize += pwsData->len;
		pwsData++;
		dwIndex++;
	}
	*Result = dwSize;
	dwSize += Message->Control.len;
	*Size = dwSize;
	return(win_malloc(dwSize));
}
PVOID 
SockPushData(PVOID Data, LPWSABUF Buffers, DWORD Count)
{
	DWORD dwIndex = 0;

	while (dwIndex < Count){
		win_memcpy(Data, Buffers->buf, Buffers->len);
		Data += Buffers->len;
		Buffers++;
		dwIndex++;
	}
	return(Data);
}
PVOID 
SockPopData(LPWSABUF Buffers, PVOID Data, DWORD Count)
{
	DWORD dwIndex = 0;

	while (dwIndex < Count){
		win_memcpy(Buffers->buf, Data, Buffers->len);
		Data += Buffers->len;
		Buffers++;
		dwIndex++;
	}
	return(Data);
}

/****************************************************/

BOOL 
pipe_socket(INT Family, INT Type, INT Protocol, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;

	Result->Handle = INVALID_HANDLE_VALUE;
	Result->FSType = FS_TYPE_PIPE;
	Result->FileType = WIN_VSOCK;
	Result->DeviceType = DEV_CLASS_CPU;
	Result->Access = GENERIC_READ + GENERIC_WRITE;
	return(TRUE);
}
BOOL 
pipe_bind(WIN_VNODE *Node, LPSOCKADDR Name, INT Length)
{
	BOOL bResult = FALSE;
	WIN_MODE wMode = {WIN_VSOCK, WIN_S_IRW, WIN_S_IRW, WIN_S_IRW, 0};
	WCHAR szName[MAX_NAME] = {0};
	OVERLAPPED ovl = {0, 0, 0, 0, __PipeEvent};
	DWORD dwAttribs = FILE_FLAG_OVERLAPPED + PIPE_READMODE_MESSAGE;

	if (!PipeCreateFile(PipeCreateName(szName), dwAttribs, __PipeEvent, Node)){
		SockError(GetLastError());
	}else if (!pipe_mknod((LPWSTR)Name->sa_data, szName, &wMode)){
		SockError(GetLastError());
	}else{
		ConnectNamedPipe(Node->Handle, &ovl);
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
pipe_connect(WIN_VNODE *Node, CONST LPSOCKADDR Name, INT Length)
{
	BOOL bResult = FALSE;
	HANDLE hResult;
	WIN_INODE iNode;
	DWORD dwSize = sizeof(WIN_INODE);

//__PRINTF("  pipe_connect(%ls)\n", Name->sa_data)
	hResult = CreateFileW((LPWSTR)Name->sa_data, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hResult == INVALID_HANDLE_VALUE){
		SockError(GetLastError());
	}else if (!ReadFile(hResult, &iNode, dwSize, &dwSize, NULL)){
		WIN_ERR("ReadFile(%s): %s\n", Name->sa_data, win_strerror(GetLastError()));
	}else if (!CloseHandle(hResult)){
		WIN_ERR("CloseHandle(%d): %s\n", hResult, win_strerror(GetLastError()));
	}else if (!PipeOpenFile(iNode.Name, __PipeEvent, Node)){
		SockError(GetLastError());
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
pipe_socketpair(INT Domain, INT Mode, INT Protocol, WIN_VNODE Result[2])
{
	BOOL bResult = FALSE;
	WCHAR szName[MAX_GUID];
//	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, FALSE};	/* ftpd.exe */
//	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};	/* sshd.exe -d */
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (!PipeCreateFile(PipeCreateName(szName), Mode, hEvent, &Result[0])){
		SockError(GetLastError());
	}else if (!PipeOpenFile(szName, hEvent, &Result[1])){
		SockError(GetLastError());
	}else{
		Result[0].FileType = WIN_VSOCK;
		Result[0].DeviceType = DEV_CLASS_CPU;
		Result[1].FileType = WIN_VSOCK;
		Result[1].DeviceType = DEV_CLASS_CPU;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
pipe_sendto(WIN_VNODE *Node, LPCSTR Buffer, UINT Size, DWORD Flags, DWORD *Result)
{
	BOOL bResult = FALSE;
	OVERLAPPED ovl = {0, 0, 0, 0, Node->Event};

	if (!WriteFile(Node->Handle, Buffer, Size, Result, &ovl)){
		SockError(GetLastError());
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
pipe_recvfrom(WIN_VNODE *Node, LPSTR Buffer, UINT Size, DWORD Flags, DWORD *Result)
{
	BOOL bResult = FALSE;

	if (!sock_read(Node, Buffer, Size, Result)){
		SockError(GetLastError());
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
pipe_sendmsg(WIN_VNODE *Node, WSAMSG *Message, DWORD Flags, DWORD *Result)
{
	BOOL bResult = FALSE;
	DWORD dwSize, dwResult;
	PVOID pvData, P;
	OVERLAPPED ovl = {0, 0, 0, 0, Node->Event};

	pvData = SockAllocData(Message, &dwSize, &dwResult);
	P = SockPushData(pvData, Message->lpBuffers, Message->dwBufferCount);
	P = SockPushData(P, &Message->Control, 1);
	if (!WriteFile(Node->Handle, pvData, dwSize, &dwSize, &ovl)){
		WIN_ERR("pipe_sendmsg(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else{
		*Result = dwResult;
		bResult = TRUE;
	}
	win_free(pvData);
	return(bResult);
}
BOOL 
pipe_recvmsg(WIN_VNODE *Node, WSAMSG *Message, DWORD *Flags, DWORD *Result)
{
	BOOL bResult = FALSE;
	DWORD dwSize, dwResult;
	PVOID pvData;

	pvData = SockAllocData(Message, &dwSize, &dwResult);
	if (!sock_read(Node, pvData, dwSize, &dwSize)){
		WIN_ERR("pipe_recvmsg(%d): %s\n", Node->Handle, win_strerror(GetLastError()));
	}else{
		pvData = SockPopData(Message->lpBuffers, pvData, Message->dwBufferCount);
		SockPopData(&Message->Control, pvData, 1);
		*Result = dwResult;
		bResult = TRUE;
	}
	win_free(pvData);
	return(bResult);
}
BOOL 
pipe_getsockopt(WIN_VNODE *Node, INT Level, INT Name, CHAR *Value, INT *Length)
{
	BOOL bResult = TRUE;

	switch (Name){
		case SO_RCVBUF:
		case SO_SNDBUF:
			*(UINT *)Value = WIN_PIPE_BUF;
			*Length = sizeof(UINT);
			break;
		default:
			WSASetLastError(WSAENOPROTOOPT);
			bResult = FALSE;
	}
	return(bResult);
}
BOOL 
pipe_setsockopt(WIN_VNODE *Node, INT Level, INT Name, CONST CHAR *Value, INT Length)
{
	BOOL bResult = TRUE;

	switch (Name){
		case SO_RCVBUF:
		case SO_SNDBUF:
//			bResult = (*(UINT *)Value == WIN_PIPE_BUF);
			break;
		default:
			WSASetLastError(WSAENOPROTOOPT);
			bResult = FALSE;
	}
	return(bResult);
}
