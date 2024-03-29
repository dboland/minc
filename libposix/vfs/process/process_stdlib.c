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

VOID 
ProcInitStreams(WIN_VNODE Node[])
{
	LONG lIndex = 0;

	/* standard stream pseudo devices (sort.exe)
	 */
	while (lIndex < 3){
		SetStdHandle(STD_INPUT_HANDLE - lIndex, Node->Handle);
		Node++;
		lIndex++;
	}
}
VOID 
ProcInitLimits(DWORDLONG Limits[])
{
	Limits[WIN_RLIMIT_CPU] = WIN_RLIM_INFINITY;
	Limits[WIN_RLIMIT_FSIZE] = WIN_FSIZE_MAX;
	Limits[WIN_RLIMIT_DATA] = 0x00100000L;		/* SizeOfHeapReserve? */
	Limits[WIN_RLIMIT_STACK] = WIN_STACKSIZE;
	Limits[WIN_RLIMIT_CORE] = WIN_RLIM_INFINITY;
	Limits[WIN_RLIMIT_RSS] = 0x7FFFFFFFL;		/* ksh.exe ("memory") */
	Limits[WIN_RLIMIT_MEMLOCK] = 0x7FFFFFFFL;
	Limits[WIN_RLIMIT_NPROC] = WIN_CHILD_MAX;
	Limits[WIN_RLIMIT_NOFILE] = WIN_OPEN_MAX;
}
VOID 
ProcInitChannels(WIN_VNODE Result[])
{
	DWORD dwIndex = 0;

	while (dwIndex < WIN_OPEN_MAX){
		Result->FileId = dwIndex++;
		Result++;
	}
}

/************************************************************/

WIN_TASK *
proc_init(WIN_SIGPROC SignalProc)
{
	STARTUPINFO si = {0};
	WIN_TASK *pwTask = NULL;

	__SignalProc = SignalProc;
	SetUnhandledExceptionFilter(SigExceptionProc);
	SetConsoleCtrlHandler(ConsoleControlHandler, TRUE);
	SetLastError(ERROR_SUCCESS);
	GetStartupInfo(&si);
	if (si.dwFlags & STARTF_PS_EXEC){
		pwTask = &__Tasks[si.dwX];
		ProcInitStreams(pwTask->Node);
		pwTask->Flags |= WIN_PS_INEXEC;
	}else{
		pwTask = ProcCreateTask(0);
		pwTask->Flags |= WIN_PS_SYSTEM;
		vfs_namei(GetStdHandle(STD_INPUT_HANDLE), 0, &pwTask->Node[0]);
		vfs_namei(GetStdHandle(STD_OUTPUT_HANDLE), 1, &pwTask->Node[1]);
		vfs_namei(GetStdHandle(STD_ERROR_HANDLE), 2, &pwTask->Node[2]);
		win_getcwd(PSTRING(pwTask).Path);
		win_geteuid(&pwTask->UserSid);
		win_getegid(&pwTask->GroupSid);
		pwTask->FileMask = 0022;
		ProcInitChannels(pwTask->Node);
		ProcInitLimits(pwTask->Limit);
		win_chdir(L"\\");	/* make sure CWD is at mount point */
	}
//	if (vfs_setugid(pwTask)){
//		pwTask->IsSetUGid = 1;
//	}
	__TaskId = pwTask->TaskId;
	__CTTY = &__Terminals[pwTask->TerminalId];
	return(pwTask);
}
