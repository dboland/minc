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

VOID 
ProcDupChannels(WIN_VNODE Nodes[], WIN_VNODE Result[])
{
	DWORD dwIndex = 0;

	while (dwIndex < WIN_OPEN_MAX){
		Result->FileId = dwIndex;
		if (Nodes->Access){
			/* don't clear FD_CLOEXEC */
			vfs_F_DUPFD(Nodes, Nodes->CloseExec, Result);
		}
		dwIndex++;
		Nodes++;
		Result++;
	}
}

/****************************************************/

WIN_TASK *
proc_dup(WIN_TASK *Parent, WIN_THREAD_STRUCT *Thread)
{
	WIN_TASK *ptResult = ProcCreateTask(Parent->GroupId);

	ptResult->State = WIN_SIDL;
	ptResult->Flags |= Thread->Flags;
	if (Parent->TracePoints & WIN_KTRFAC_INHERIT){
		ptResult->TraceHandle = Parent->TraceHandle;
		ptResult->TracePoints = Parent->TracePoints;
		ptResult->Flags |= WIN_PS_TRACED;
	}
	ptResult->Handle = Thread->Handle;
	ptResult->ParentId = Parent->TaskId;
	ptResult->Depth = Parent->Depth + 1;
	ptResult->GroupId = Parent->GroupId;
	ptResult->SessionId = Parent->SessionId;
	ptResult->TerminalId = Parent->TerminalId;
	ptResult->MountId = Parent->MountId;
	ptResult->UserSid = Parent->UserSid;
	ptResult->GroupSid = Parent->GroupSid;
	ptResult->FileMask = Parent->FileMask;
	ptResult->IsSetUGid = Parent->IsSetUGid;
	ptResult->RealUid = Parent->RealUid;
	ptResult->SavedUid = Parent->SavedUid;
	ptResult->RealGid = Parent->RealGid;
	ptResult->SavedGid = Parent->SavedGid;
	ptResult->ProcMask = Parent->ProcMask;
	win_memcpy(ptResult->Limit, Parent->Limit, WIN_RLIM_NLIMITS * sizeof(DWORDLONG));
	win_memcpy(ptResult->AtExit, Parent->AtExit, WIN_ATEXIT_MAX * sizeof(WIN_ATEXITPROC));
	win_memcpy(ptResult->Action, Parent->Action, WIN_NSIG * sizeof(WIN_SIGACTION));
	win_memcpy(&PSTRING(ptResult), &PSTRING(Parent), sizeof(WIN_PSTRING));
	ProcDupChannels(Parent->Node, ptResult->Node);
	return(ptResult);
}
BOOL 
proc_close(WIN_TASK *Task)
{
	BOOL bResult = FALSE;

	/* In very specific (yet unknown) cases CreateProcess() is 
	 * actually *faster* than CreateThread() (building perl.exe)
	 */
	Task->ParentId = 0;		/* relinquish ownership */
	if (!Task->Handle){
		SetLastError(ERROR_INVALID_THREAD_ID);
	}else if (CloseHandle(Task->Handle)){
		bResult = TRUE;
	}else{				/* tar.exe */
		WIN_ERR("proc_close(%d): %s\n", Task->Handle, win_strerror(GetLastError()));
	}
	Task->Flags |= WIN_PS_NOZOMBIE;	/* ready to be reclaimed */
	return(bResult);
}
