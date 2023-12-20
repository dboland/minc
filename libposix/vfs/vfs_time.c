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

//#define OpenWaitableTimer __AW(OpenWaitableTimer)

/****************************************************/

VOID CALLBACK 
TimeProc(PVOID Param, DWORD LowValue, DWORD HighValue)
{
	WIN_TASK *pwTask = Param;
//	DWORDLONG dwlTime = (HighValue * 0x100000000) + LowValue;

	/* This procedure will not be called when including the handle 
	 * created by CreateWaitableTimer() in one of the 
	 * WaitFor*Ex() functions.
	 */
	if (!PostThreadMessage(pwTask->ThreadId, WM_TIMER, LowValue, HighValue)){
		WIN_ERR("PostThreadMessage(%d): %s\n", pwTask->ThreadId, win_strerror(GetLastError()));
	}
}

/****************************************************/

BOOL 
vfs_clock_gettime_REALTIME(DWORDLONG *Result)
{
	GetSystemTimeAsFileTime((FILETIME *)Result);
	return(TRUE);
}
BOOL 
vfs_clock_gettime_MONOTONIC(LARGE_INTEGER Frequency, DWORDLONG *Result)
{
	BOOL bResult = TRUE;
	LARGE_INTEGER liCount;

	if (QueryPerformanceCounter(&liCount)){
		*Result = Frequency.QuadPart * liCount.QuadPart;
	}else{
		bResult = FALSE;
	}
	return(bResult);
}

/****************************************************/

BOOL 
vfs_clock_gettime(DWORD ClockId, DWORDLONG *Result)
{
	BOOL bResult = FALSE;

	switch (ClockId){
		case WIN_CLOCK_REALTIME:	/* gmake.exe */
			bResult = vfs_clock_gettime_REALTIME(Result);
			break;
		case WIN_CLOCK_MONOTONIC:	/* git.exe */
			bResult = vfs_clock_gettime_MONOTONIC(__Globals[WIN_KERN_CLOCKRATE], Result);
			break;
		case WIN_CLOCK_VIRTUAL:
		case WIN_CLOCK_PROCESS_CPUTIME_ID:
		case WIN_CLOCK_THREAD_CPUTIME_ID:
		case WIN_CLOCK_UPTIME:
			SetLastError(ERROR_NOT_SUPPORTED);
			break;
		default:
			SetLastError(ERROR_BAD_ARGUMENTS);
	}
	return(bResult);
}
BOOL 
vfs_setitimer(WIN_TASK *Task, LONG *Interval, DWORDLONG *TimeOut)
{
	BOOL bResult = FALSE;
	LARGE_INTEGER liTimeOut;
	DWORDLONG dwlTimeOut = *TimeOut;
	LONG lInterval = *Interval;
	DWORDLONG dwlTicks = 0LL;
	LONGLONG llRemain;
	HANDLE hTimer = NULL;

	liTimeOut.QuadPart = (LONGLONG)(dwlTimeOut * -0.01);	/* 100-nanosecond intervals */
	if (!vfs_clock_gettime(WIN_CLOCK_MONOTONIC, &dwlTicks)){
		return(FALSE);
	}else if (!ProcGetTimer(Task, &hTimer)){
		return(FALSE);
	}else if (!dwlTimeOut){
		lInterval = 0;
		Task->Timer = NULL;
		bResult = CloseHandle(hTimer);
	}else if (!SetWaitableTimer(hTimer, &liTimeOut, lInterval, TimeProc, Task, FALSE)){
		WIN_ERR("SetWaitableTimer(%d): %s\n", hTimer, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	llRemain = (LONGLONG)(Task->Ticks - dwlTicks);
	if (llRemain < 0){
		*TimeOut = 0LL;
	}else{
		*TimeOut = llRemain;
	}
	*Interval = Task->Interval;
	Task->Interval = lInterval;
	Task->Ticks = (DWORDLONG)(dwlTicks + dwlTimeOut);
//VfsDebugTimer(Task, "vfs_setitimer");
	return(bResult);
}
BOOL 
vfs_nanosleep(WIN_TASK *Task, DWORD TimeOut, DWORD *Remain)
{
	BOOL bResult = FALSE;
	DWORDLONG dwlTime = (DWORDLONG)(TimeOut + GetTickCount());
	LONGLONG llRemain = 0;
//	HANDLE hObjects[MAXIMUM_WAIT_OBJECTS];
	DWORD dwStatus;

	dwStatus = WaitForSingleObjectEx(__Interrupt, TimeOut, TRUE);
	if (dwStatus == WAIT_FAILED){
		WIN_ERR("WaitForSingleObjectEx(%d): %s\n", __Interrupt, win_strerror(WSAGetLastError()));
	}else if (!proc_poll()){
		bResult = TRUE;
	}
	llRemain = (LONGLONG)(dwlTime - GetTickCount());
	if (llRemain > 0){
		*Remain = llRemain;
	}else{
		*Remain = 0;
	}
	return(bResult);
}
BOOL 
vfs_utimes(WIN_NAMEIDATA *Path, FILETIME FileTime[2])
{
	BOOL bResult = FALSE;

	switch (Path->FSType){
		case FS_TYPE_DISK:
		case FS_TYPE_PDO:
			bResult = disk_utimes(Path, FileTime);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
			break;
	}
	return(bResult);
}
