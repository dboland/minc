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

#include <sys/time.h>

/****************************************************/

FILETIME 
timeval_win(const struct timeval *timev)
{
	DWORDLONG dwlTime = (DWORDLONG)timev->tv_sec;
	FILETIME ftResult;

	dwlTime *= 10000000LL;			/* 100-nanosecond intervals */
	dwlTime += 116444736000000000LL;		/* epoch */
	dwlTime += (timev->tv_usec * 10);		/* microseconds */
	*(DWORDLONG *)&ftResult = dwlTime;
	return(ftResult);
}
FILETIME *
utimeval_win(FILETIME FileTime[2], const struct timeval tv[2])
{
	FILETIME fTime;

	if (!tv){
		GetSystemTimeAsFileTime(&fTime);
		FileTime[0] = fTime;
		FileTime[1] = fTime;
	}else{
		/* cp.exe -p */
		FileTime[0] = timeval_win(&tv[0]);	/* access */
		FileTime[1] = timeval_win(&tv[1]);	/* mod */
	}
	return(FileTime);
}
FILETIME 
timespec_win(const struct timespec *times)
{
	DWORDLONG dwlTime = times->tv_sec;
	FILETIME ftResult = {0, 0};

	if (times->tv_nsec == UTIME_NOW){
		GetSystemTimeAsFileTime(&ftResult);
	}else if (times->tv_nsec != UTIME_OMIT){
		dwlTime *= 10000000LL;		/* 100-nanosecond intervals */
		dwlTime += 116444736000000000LL;	/* epoch */
		dwlTime += (times->tv_nsec * 0.1);
		*(DWORDLONG *)&ftResult = dwlTime;
	}
	return(ftResult);
}
FILETIME *
utimespec_win(FILETIME FileTime[2], const struct timespec times[2])
{
	FILETIME fTime;

	if (!times){
		GetSystemTimeAsFileTime(&fTime);
		FileTime[0] = fTime;
		FileTime[1] = fTime;
	}else{
		FileTime[0] = timespec_win(&times[0]);
		FileTime[1] = timespec_win(&times[1]);
	}
	return(FileTime);
}

/****************************************************/

int 
clock_gettime_REALTIME(struct timespec *tp, DWORDLONG Time)
{
	Time -= 116444736000000000LL;			/* epoch (100-nanosecond intervals) */
	tp->tv_sec = (time_t)(Time * 0.0000001);		/* seconds (gcc needs cast!) */
	Time *= 100;
	tp->tv_nsec = Time - (tp->tv_sec * 1000000000);	/* nanoseconds */
	return(0);
}
int 
clock_gettime_MONOTONIC(struct timespec *tp, DWORDLONG Time)
{
	DWORD dwTime = Time & 0xFFFFFFFF;

	tp->tv_sec = (time_t)(Time * 0.000000001);	/* seconds (gcc needs cast!) */
//	tp->tv_nsec = dwTime % 1000000000;			/* nanoseconds */
	tp->tv_nsec = Time - (tp->tv_sec * 1000000000);	/* nanoseconds */
	return(0);
}
int 
setitimer_REAL(WIN_TASK *Task, LONG *Interval, DWORDLONG *TimeOut)
{
	int result = 0;

	if (!vfs_setitimer(Task, Interval, TimeOut)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}

/****************************************************/

int 
sys_nanosleep(call_t call, const struct timespec *req, struct timespec *rem)
{
	int result = 0;
	DWORD dwRemain;
	DWORD dwMillisecs;
	WIN_TASK *pwTask = call.Task;

	/* __int64_t (%I64d) */
	dwMillisecs = (req->tv_sec & 0xBFFFFFFF) * 1000;	/* limit to DWORD bits */
	dwMillisecs += req->tv_nsec * 0.000001;			/* nanoseconds */
	if (req->tv_nsec > 999999999){
		result = -EINVAL;
	}else if (!vfs_nanosleep(pwTask, dwMillisecs, &dwRemain)){
		result -= errno_posix(GetLastError());
	}
	if (rem){
		rem->tv_sec = dwRemain * 0.001;
		rem->tv_nsec = (dwRemain % 1000) * 1000000;	/* nanoseconds */
	}
	return(result);
}
int 
sys_gettimeofday(call_t call, struct timeval *tv, struct timezone *tz)
{
	int result = 0;
	DWORDLONG dwlTime;

	if (!tv){
		result = -EINVAL;
	}else{
		GetSystemTimeAsFileTime((FILETIME *)&dwlTime);
		dwlTime -= 116444736000000000LL;		/* epoch */
		dwlTime *= 0.1;					/* 100-nanosecond intervals */
		tv->tv_sec = (time_t)(dwlTime * 0.000001);	/* seconds (date.exe) */
		tv->tv_usec = dwlTime - (tv->tv_sec * 1000000);	/* microseconds (ab.exe) */
		result = 0;
	}
	return(result);
}
int 
sys_settimeofday(call_t call, const struct timeval *tp, const struct timezone *tzp)
{
	int result = 0;

	if (!win_settimeofday(tp->tv_sec, tp->tv_usec * 0.001)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_clock_gettime(call_t call, clockid_t clk_id, struct timespec *tp)
{
	int result = 0;
	DWORDLONG dwlTime = 0;

	if (!vfs_clock_gettime(clk_id, &dwlTime)){
		result -= errno_posix(GetLastError());
	}else if (clk_id == CLOCK_REALTIME){			/* git.exe */
		result = clock_gettime_REALTIME(tp, dwlTime);
	}else if (clk_id == CLOCK_MONOTONIC){			/* git.exe */
		result = clock_gettime_MONOTONIC(tp, dwlTime);
	}
//msvc_printf("clock_gettime(%I64d): tv_sec(%I64d) tv_nsec(%lu)\n", 
//		dwlTime, tp->tv_sec, tp->tv_nsec);
	return(result);
}
int 
sys_setitimer(call_t call, int which, const struct itimerval *restrict value, struct itimerval *restrict ovalue)
{
	int result = 0;
	LONG lInterval = 0;
	DWORDLONG dwlTimeOut = 0;
	DWORD dwResult;
	WIN_TASK *pwTask = call.Task;

	/* OpenBSD's profiler passes NULL */
	if (value){
		lInterval = (value->it_interval.tv_sec & 0xBFFFFFFF) * 1000;
		lInterval += value->it_interval.tv_usec * 0.001;
		dwlTimeOut = value->it_value.tv_usec * 1000;
		dwlTimeOut += (DWORDLONG)(value->it_value.tv_sec * 1000000000);
	}
//__PRINTF("sys_setitimer(%d): Interval(%d) TimeOut(%I64d)\n", task->pid, lInterval, dwlTimeOut)
	if (which == ITIMER_REAL){
		result = setitimer_REAL(pwTask, &lInterval, &dwlTimeOut);
	}else{
		return(-EOPNOTSUPP);
	}
	if (ovalue){
		dwResult = dwlTimeOut & 0xFFFFFFFF;
		ovalue->it_value.tv_sec = (time_t)(dwlTimeOut * 0.000000001);
		ovalue->it_value.tv_usec = (dwResult % 1000000000) * 0.001;
//__PRINTF("sys_setitimer(out:value): sec(%I64d) usec(%lu)\n", ovalue->it_value.tv_sec, ovalue->it_value.tv_usec)
		ovalue->it_interval.tv_sec = (time_t)(lInterval * 0.001);
		ovalue->it_interval.tv_usec = (lInterval % 1000) * 1000;
	}
	return(result);
}
int 
sys_futimens(call_t call, int fd, const struct timespec times[2])
{
	int result = 0;
	FILETIME fTime[2];
	WIN_TASK *pwTask = call.Task;

	if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!disk_futimes(&pwTask->Node[fd], utimespec_win(fTime, times))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
__utimensat(WIN_TASK *Task, int dirfd, const char *pathname, FILETIME Time[2], int flags)
{
	int result = 0;
	WIN_NAMEI wPath;

	if (!vfs_utimes(pathat_win(&wPath, dirfd, pathname, flags), Time)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_utimensat(call_t call, int dirfd, const char *path, const struct timespec times[2], int flags)
{
	FILETIME fTime[2];

	return(__utimensat(call.Task, dirfd, path, utimespec_win(fTime, times), flags));
}
int 
sys_utimes(call_t call, const char *path, const struct timeval times[2])
{
	FILETIME fTime[2];

	return(__utimensat(call.Task, AT_FDCWD, path, utimeval_win(fTime, times), 0));
}
