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

/************************************************************/

int 
runcmd(char *argv[])
{
	int result = -1;
	CHAR szPath[PATH_MAX];
	WIN_TASK *pwTask = &__Tasks[CURRENT];

	win_wcstombs(szPath, pwTask->Path.Name, PATH_MAX);
	if (!win_execve(argv_win(pwTask, *argv, argv), szPath)){
		pwTask->Error = errno_posix(GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
/* int 
group_member(gid_t gid)
{
	int result = -1;
	SID8 sid;
	WIN_TASK *pwTask = &__Tasks[CURRENT];

	if (!gid){
		gid = WIN_ROOT_GID;
	}
	if (!win_group_member(rid_win(&sid, gid))){
		pwTask->Error = errno_posix(GetLastError());
	}else{
		result = 0;
	}
	return(result);
} */
/* int 
futimesat(int dirfd, const char *pathname, const struct timeval times[2])
{
	int result = -1;
	FILETIME fTime[2];
	WIN_NAMEI wPath;
	WIN_TASK *pwTask = &__Tasks[CURRENT];

	if (!vfs_utimes(pathat_win(&wPath, dirfd, pathname, AT_SYMLINK_FOLLOW), utimeval_win(fTime, times))){
		pwTask->Error = errno_posix(GetLastError());
	}else{
		result = 0;
	}
	return(result);
} */
