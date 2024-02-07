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
SysCloseExec(WIN_VNODE Nodes[])
{
	DWORD dwIndex = 0;

	while (dwIndex < WIN_OPEN_MAX){
		if (Nodes->CloseExec){
			vfs_close(Nodes);
		}
		dwIndex++;
		Nodes++;
	}
	return(TRUE);
}
VOID 
SysInheritChannels(HANDLE Process, WIN_VNODE Result[])
{
	DWORD dwIndex = 0;

	while (dwIndex < WIN_OPEN_MAX){
		if (Result->Access){
			if (!(Result->Flags & HANDLE_FLAG_INHERIT)){
				vfs_F_INHERIT(Result, Process);
			}
		}
		Result++;
		dwIndex++;
	}
}

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
BOOL 
vfs_setugid(WIN_TASK *Task)
{
	BOOL bResult = FALSE;
	WCHAR szPath[MAX_PATH];
	WIN_VATTR wStat = {0};

	if (!GetModuleFileNameW(NULL, szPath, MAX_PATH)){
		return(FALSE);
	}else if (!DiskStatFile(szPath, FILE_ATTRIBUTE_NORMAL, &wStat)){
		return(FALSE);
	}
	if (wStat.Mode.Special & WIN_S_ISUID){
		bResult = vfs_seteuid(Task, &wStat.UserSid);
	}
	if (wStat.Mode.Special & WIN_S_ISGID){
		bResult = vfs_setegid(Task, &wStat.GroupSid);
	}
	return(bResult);
}
BOOL 
vfs_execve(WIN_TASK *Task, LPSTR Command, PVOID Environ)
{
	BOOL bResult = FALSE;
	STARTUPINFO si = {0};
	HANDLE hToken = NULL;
	PROCESS_INFORMATION pi = {0};
	DWORD dwAccess = TOKEN_QUERY + TOKEN_DUPLICATE + TOKEN_ASSIGN_PRIMARY;
	HANDLE hThread;
	WIN_OBJECT_CONTROL wControl;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), &wControl.Security, FALSE};
	DWORD dwFlags = NORMAL_PRIORITY_CLASS;

	si.cb = sizeof(STARTUPINFO);
	si.lpDesktop = "";		/* Vista */
	si.dwFlags = STARTF_PS_EXEC;
	si.dwX = Task->TaskId;
	SysCloseExec(Task->Node);
	if (!win_cap_get_proc(dwAccess, TokenPrimary, &hToken)){
		WIN_ERR("win_cap_get_proc(%s): %s\n", Command, win_strerror(GetLastError()));
	}else if (!AclCreateControl(WIN_P_IRWX | TOKEN_IMPERSONATE, &wControl)){
		WIN_ERR("AclCreateControl(%s): %s\n", Command, win_strerror(GetLastError()));
	}else if (CreateProcessAsUser(hToken, NULL, Command, &sa, NULL, TRUE, dwFlags, Environ, NULL, &si, &pi)){
		Task->Flags |= WIN_PS_EXEC;
		Task->ThreadId = pi.dwThreadId;
		hThread = Task->Handle;
		if (Task->ProcessId != GetCurrentProcessId()){	/* not forked (ktrace.exe) */
			Task->Handle = win_F_DISINHERIT(pi.hThread, Task->ProcessId);
		}else{
			Task->Handle = pi.hThread;
		}
		SysInheritChannels(pi.hProcess, Task->Node);
		CloseHandle(hThread);
		CloseHandle(pi.hProcess);
		/* https://man7.org/linux/man-pages/man2/execve.2.html */
		ZeroMemory(Task->Action, WIN_NSIG * sizeof(WIN_SIGACTION));
		/* https://pubs.opengroup.org/onlinepubs/9699919799/functions/atexit.html */
		ZeroMemory(Task->AtExit, WIN_ATEXIT_MAX * sizeof(WIN_ATEXITPROC));
		Task->State = WIN_SRUN;
		bResult = TRUE;
	}else{
		WIN_ERR("CreateProcessAsUser(%s): %s\n", Command, win_strerror(GetLastError()));
	}
	CloseHandle(hToken);
	win_free(Command);
	win_free(Environ);
	return(bResult);
}
