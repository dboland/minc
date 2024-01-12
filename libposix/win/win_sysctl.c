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
win_KERN_CLOCKRATE(DWORDLONG *Result)
{
	BOOL bResult;
	LARGE_INTEGER liFrequency;

	if (bResult = QueryPerformanceFrequency(&liFrequency)){
		liFrequency.QuadPart *= 0.001;					/* picoseconds */
		*Result = (DWORDLONG)(liFrequency.LowPart % 1000000000);	/* nanoseconds */
	}
	return(bResult);
}
BOOL 
win_KERN_TIMECOUNTER_TICK(DWORDLONG *Result)
{
	BOOL bResult;
	LARGE_INTEGER liCount;

	if (bResult = QueryPerformanceCounter(&liCount)){
		*Result = (DWORDLONG)(liCount.QuadPart * 0.000000001);	/* seconds */
	}
	return(bResult);
}
BOOL 
win_KERN_HOSTNAME(LPSTR Current, LPCSTR New, DWORD Size)
{
	BOOL bResult = FALSE;

	if (Current){
		bResult = GetComputerNameEx(ComputerNameDnsFullyQualified, Current, &Size);
	}else if (New){
		bResult = SetComputerNameExA(ComputerNamePhysicalDnsHostname, New);
	}else{
		SetLastError(ERROR_BAD_ARGUMENTS);
	}
	return(bResult);
}
BOOL 
win_KERN_DOMAINNAME(LPSTR Current, LPCSTR New, DWORD Size)
{
	BOOL bResult = FALSE;

	if (Current){
		bResult = GetComputerNameEx(ComputerNameDnsDomain, Current, &Size);
	}else if (New){
		bResult = SetComputerNameExA(ComputerNamePhysicalDnsDomain, New);
	}else{
		SetLastError(ERROR_BAD_ARGUMENTS);
	}
	return(bResult);
}
BOOL 
win_KERN_PROC(DWORD ThreadId, WIN_KINFO_PROC *Result)
{
	BOOL bResult = FALSE;
	HANDLE hThread = NULL;
	PFILETIME pftCreation = &Result->Created;
	PFILETIME pftExit = &Result->Exited;
	PFILETIME pftKernel = &Result->Kernel;
	PFILETIME pftUser = &Result->User;

	if (!(hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, ThreadId))){
		return(FALSE);
	}else if (GetThreadTimes(hThread, pftCreation, pftExit, pftKernel, pftUser)){
		bResult = CloseHandle(hThread);
	}else{
		WIN_ERR("GetThreadTimes(%d): %s\n", hThread, win_strerror(GetLastError()));
	}
	return(bResult);
}
