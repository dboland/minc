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

#include <utime.h>

/****************************************************/

FILETIME 
time_win(const time_t time)
{
	DWORDLONG dwlTime = (DWORDLONG)time;
	FILETIME ftResult;

	dwlTime *= 10000000LL;		/* 100-nanosecond intervals */
	dwlTime += 116444736000000000LL;	/* epoch */
	*(DWORDLONG *)&ftResult = dwlTime;
	return(ftResult);
}
FILETIME *
utimbuf_win(FILETIME FileTime[2], const struct utimbuf *times)
{
	FILETIME fTime;

	if (!times){
		GetSystemTimeAsFileTime(&fTime);
		FileTime[0] = fTime;
		FileTime[1] = fTime;
	}else{
		FileTime[0] = time_win(times->actime);
		FileTime[1] = time_win(times->modtime);
	}
	return(FileTime);
}

/****************************************************/

int 
sys_utime(call_t call, const char *path, const struct utimbuf *times)
{
	int result = -1;
	WIN_NAMEIDATA wPath;
	FILETIME fTime[2];

	if (!vfs_utimes(path_win(&wPath, path, 0), utimbuf_win(fTime, times))){
		__errno_posix(call.Task, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
int 
sys_futimes(call_t call, int fd, const struct timeval tv[2])
{
	int result = -1;
	FILETIME fTime[2];
	WIN_TASK *pwTask = call.Task;

	if (fd < 0 || fd >= OPEN_MAX){
		__errno_posix(pwTask, ERROR_INVALID_HANDLE);
	}else if (!disk_futimes(&pwTask->Node[fd], utimeval_win(fTime, tv))){
		__errno_posix(pwTask, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
