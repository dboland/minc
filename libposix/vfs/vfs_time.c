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
LONGLONG 
TimeGetTickCount(VOID)
{
	return((LONGLONG)(GetTickCount() * 10000));
}

/****************************************************/

BOOL 
vfs_setitimer(WIN_TASK *Task, LONG *Interval, DWORDLONG *TimeOut)
{
	BOOL bResult = FALSE;
	LARGE_INTEGER liTimeOut;
	DWORDLONG dwlTimeOut = *TimeOut;
	LONG lInterval = *Interval;
	DWORDLONG dwlTicks = 0LL;
	LONGLONG llRemain;
	HANDLE hTimer;
	PTIMERAPCROUTINE ptProc = TimeProc;

	/* If not set to NULL, the completion routine would be called
	 * one last time when resetting the timer (ftp.exe).
	 */
	if (!dwlTimeOut){
		ptProc = NULL;
	}
	liTimeOut.QuadPart = (LONGLONG)(dwlTimeOut * -0.01);	/* 100-nanosecond intervals */
	if (!win_clock_gettime_MONOTONIC(&dwlTicks)){
		return(FALSE);
	}else if (!proc_getitimer(Task, &hTimer)){
		return(FALSE);
	}else if (!SetWaitableTimer(hTimer, &liTimeOut, lInterval, ptProc, Task, FALSE)){
		WIN_ERR("SetWaitableTimer(%d): %s\n", hTimer, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	llRemain = Task->Ticks - dwlTicks;
	if (llRemain > 0){
		*TimeOut = llRemain;
	}else{
		*TimeOut = 0LL;
	}
	*Interval = Task->Interval;
	Task->Interval = lInterval;
	Task->Ticks = dwlTicks + dwlTimeOut;
//VfsDebugTimer(Task, "vfs_setitimer");
	return(bResult);
}
BOOL 
vfs_nanosleep(DWORDLONG *TimeOut, DWORDLONG *Remain)
{
	BOOL bResult = FALSE;
	LONGLONG llTimeOut = *TimeOut;
	LONGLONG llRemain = llTimeOut + TimeGetTickCount();
	HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	HANDLE hObjects[2] = {__Interrupt, hTimer};
	LARGE_INTEGER liTimeOut;
	DWORD dwResult;

	liTimeOut.QuadPart = -llTimeOut;	/* relative time */
	SetWaitableTimer(hTimer, &liTimeOut, 0, NULL, NULL, FALSE);
	dwResult = WaitForMultipleObjectsEx(2, hObjects, FALSE, INFINITE, TRUE);
	if (dwResult == WAIT_FAILED){
		WIN_ERR("WaitForMultipleObjectsEx(%s): %s\n", win_strobj(hObjects, 2), win_strerror(GetLastError()));
	}else if (!proc_poll()){
		bResult = TRUE;
	}
	llRemain -= TimeGetTickCount();
	if (llRemain > 0){
		*Remain = llRemain;
	}else{
		*Remain = 0LL;
	}
	CloseHandle(hTimer);
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
