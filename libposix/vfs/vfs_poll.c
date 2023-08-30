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

/************************************************************/

DWORD 
PollGetObjects(HANDLE Timer, WIN_VNODE *Nodes[], HANDLE Result[])
{
	DWORD dwResult = 0;
	WIN_VNODE *pNode;

	if (Timer){
		Result[dwResult++] = Timer;
	}
	while (pNode = *Nodes++){
		Result[dwResult++] = pNode->Event;
	}
	Result[dwResult++] = __ProcEvent;
	return(dwResult);
}
DWORD 
PollNoWait(WIN_VNODE *Nodes[], WIN_POLLFD *Info[])
{
	DWORD dwResult = 0;
	WIN_VNODE *pNode;
	WIN_POLLFD *pInfo;

	while (pNode = *Nodes++){
		pInfo = *Info++;
		switch (pNode->FSType){
			case FS_TYPE_CHAR:
				dwResult += char_poll(pNode, pInfo);
				break;
			case FS_TYPE_PIPE:
				dwResult += pipe_poll(pNode, pInfo);
				break;
			case FS_TYPE_DEVICE:
				dwResult += dev_poll(pNode->Device, pInfo);
				break;
			case FS_TYPE_MAILSLOT:
				dwResult += mail_poll(pNode->Handle, pInfo);
				break;
			case FS_TYPE_DISK:	/* git.exe (format-patch) */
				dwResult += disk_poll(pNode->Handle, pInfo);
				break;
			case FS_TYPE_WINSOCK:
				dwResult += ws2_poll(pNode, pInfo);
				break;
			default:
				SetLastError(ERROR_BAD_FILE_TYPE);
				pInfo->Result = WIN_POLLERR;
				dwResult++;
		}
	}
	return(dwResult);
}
BOOL 
PollWait(WIN_TASK *Task, WIN_VNODE *Nodes[], DWORD *TimeOut)
{
	BOOL bResult = FALSE;
	DWORD dwStatus;
	LONGLONG llTime = (LONGLONG)GetTickCount();
	HANDLE hObjects[WSA_MAXIMUM_WAIT_EVENTS];
	DWORD dwCount = PollGetObjects(Task->Timer, Nodes, hObjects);
	DWORD dwTimeOut = *TimeOut;

	if (dwTimeOut != INFINITE){
		llTime += (LONGLONG)dwTimeOut;
	}
	dwStatus = WSAWaitForMultipleEvents(dwCount, hObjects, FALSE, dwTimeOut, TRUE);
	if (dwStatus == WSA_WAIT_FAILED){
		WIN_ERR("WSAWaitForMultipleEvents(%s): %s\n", win_strobj(hObjects, dwCount), win_strerror(WSAGetLastError()));
		vfs_raise(WM_COMMAND, CTRL_ABORT_EVENT, 0);
	}else{
		bResult = TRUE;
	}
	llTime -= (LONGLONG)GetTickCount();
	if (llTime > 0){
		dwTimeOut = llTime;
	}else if (dwTimeOut != INFINITE){
		dwTimeOut = 0;
	}
	*TimeOut = dwTimeOut;
	return(bResult);
}

/************************************************************/

BOOL 
vfs_poll(WIN_TASK *Task, WIN_VNODE *Nodes[], WIN_POLLFD *Info[], DWORD *TimeOut, DWORD *Result)
{
	BOOL bResult = FALSE;
	DWORD dwResult = 0;

	Task->State = WIN_SSLEEP;
	while (!bResult){
		if (dwResult = PollNoWait(Nodes, Info)){
			bResult = TRUE;
		}else if (!*TimeOut){
			bResult = TRUE;
		}else if (!PollWait(Task, Nodes, TimeOut)){
			break;
		}else if (proc_poll()){
			break;
		}
	}
	Task->State = WIN_SRUN;
	*Result = dwResult;
	return(bResult);
}
