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
vfs_seteuid(WIN_TASK *Task, SID8 *Sid)
{
	HANDLE hToken = NULL;
	BOOL bResult = FALSE;
	WIN_PWENT pwEntry;

	if (!win_getpwuid(Sid, &pwEntry)){
		return(FALSE);
	}else if (!win_cap_setuid(&pwEntry, &hToken)){
		return(FALSE);
	}else if (!SetThreadToken(NULL, hToken)){
		WIN_ERR("SetThreadToken(%ls): %s\n", pwEntry.Account, win_strerror(GetLastError()));
	}else{
		Task->UserSid = *Sid;
		bResult = CloseHandle(hToken);
	}
	return(bResult);
}
BOOL 
vfs_setegid(WIN_TASK *Task, SID8 *Sid)
{
	HANDLE hToken;
	BOOL bResult = FALSE;
	DWORD dwCount = 0;
	SID8 *grList = NULL;

	if (!win_getgroups(&grList, &dwCount)){
		return(FALSE);
	}else if (!win_cap_setgroups(Sid, grList, dwCount, &hToken)){
		WIN_ERR("win_cap_setgroups(%s): %s\n", win_strsid(Sid), win_strerror(GetLastError()));
	}else if (!SetThreadToken(NULL, hToken)){
		WIN_ERR("SetThreadToken(%s): %s\n", win_strsid(Sid), win_strerror(GetLastError()));
	}else{
		Task->GroupSid = *Sid;
		bResult = CloseHandle(hToken);
	}
	win_free(grList);
	return(bResult);
}
BOOL 
vfs_getlogin(WIN_TASK *Task, LPSTR Name, DWORD Size)
{
	BOOL bResult = FALSE;
	CHAR szBuf[MAX_NAME];
	SID_NAME_USE snType = 0;
	DWORD dwSize = MAX_NAME;

	/* This always returns logged on user, not impersonated one.
	 */
	if (!LookupAccountSid(NULL, &Task->UserSid, Name, &Size, szBuf, &dwSize, &snType)){
		WIN_ERR("LookupAccountSid(%s): %s\n", win_strsid(&Task->UserSid), win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_setlogin(WIN_TASK *Task, LPCSTR Name)
{
	BOOL bResult = FALSE;
	DWORD dwSize = sizeof(SID8);
	SID8 sid;

	if (!win_group_member(&SidAdmins)){
		SetLastError(ERROR_PRIVILEGE_NOT_HELD);
	}else if (AclLookup(Name, &sid, &dwSize)){
		bResult = vfs_seteuid(Task, &sid);
	}
	return(bResult);
}
BOOL 
vfs_pipe(WIN_VNODE Result[2])
{
	BOOL bResult = FALSE;
	HANDLE hInput, hOutput;

	if (!CreatePipe(&hInput, &hOutput, NULL, WIN_PIPE_BUF)){
		WIN_ERR("CreatePipe(%d): %s\n", WIN_PIPE_BUF, win_strerror(GetLastError()));
	}else{
		Result[0].Handle = hInput;
		Result[0].Event = __PipeEvent;
		Result[0].FSType = FS_TYPE_PIPE;
		Result[0].FileType = WIN_VFIFO;
		Result[0].DeviceType = DEV_CLASS_CPU;
		Result[0].Access = win_F_GETFL(hInput);
		Result[0].Attribs = FILE_ATTRIBUTE_NORMAL;
		Result[1].Handle = hOutput;
		Result[1].Event = __PipeEvent;
		Result[1].FSType = FS_TYPE_PIPE;
		Result[1].FileType = WIN_VFIFO;
		Result[1].DeviceType = DEV_CLASS_CPU;
		Result[1].Access = win_F_GETFL(hOutput);
		Result[1].Attribs = FILE_ATTRIBUTE_NORMAL;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_setsid(WIN_TASK *Task)
{
	BOOL bResult = FALSE;

	if (Task->TaskId == Task->GroupId){
		SetLastError(ERROR_INVALID_OPERATION);
	}else{
		Task->GroupId = Task->TaskId;
		Task->SessionId = Task->TaskId;
//		if (Task->TerminalId){
//			FreeConsole();
//		}
//		Task->TerminalId = 0;
		bResult = TRUE;
	}
	return(bResult);
}
