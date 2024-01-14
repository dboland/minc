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

#include <psapi.h>

/************************************************************/

BOOL 
ResAddUsage(HANDLE Handle, WIN_RUSAGE *Result)
{
	BOOL bResult = FALSE;
	FILETIME ftCreation, ftExit, ftKernel, ftUser;

	if (GetThreadTimes(Handle, &ftCreation, &ftExit, &ftKernel, &ftUser)){
		Result->Kernel += *(DWORDLONG *)&ftKernel;
		Result->User += *(DWORDLONG *)&ftUser;
		bResult = TRUE;
	}
	return(bResult);
}

/************************************************************/

BOOL 
vfs_getrusage_SELF(DWORD ThreadId, WIN_RUSAGE *Result)
{
	BOOL bResult = FALSE;
	HANDLE hThread;

	if (!(hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, ThreadId))){
		return(FALSE);
	}else if (ResAddUsage(hThread, Result)){
		bResult = CloseHandle(hThread);
	}
	return(bResult);
}
BOOL 
vfs_getrusage_CHILDREN(DWORD ParentId, WIN_RUSAGE *Result)
{
	BOOL bResult = TRUE;
	DWORD dwIndex = WIN_PID_INIT;
	WIN_TASK *pwTask = &__Tasks[dwIndex];

	while (dwIndex < WIN_CHILD_MAX){
		if (pwTask->ParentId == ParentId){
			bResult = ResAddUsage(pwTask->Handle, Result);
		}
		dwIndex++;
		pwTask++;
	}
	return(bResult);
}
