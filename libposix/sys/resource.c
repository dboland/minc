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

#include <sys/resource.h>

/************************************************************/

int 
rlimit_posix(WIN_TASK *Task, int resource, struct rlimit *rlp)
{
	int result = 0;

	rlp->rlim_cur = Task->Limit[resource];
	switch (resource){
		case RLIMIT_CPU:	/* su.exe */
			rlp->rlim_max = RLIM_INFINITY;
			break;
		case RLIMIT_DATA:
			rlp->rlim_max = 0x00100000L * 8;	/* SizeOfHeapReserve? */
			break;
		case RLIMIT_STACK:
			rlp->rlim_max = WIN_STACKSIZE * 8;
			break;
		case RLIMIT_RSS:	/* ksh.exe ("memory") */
		case RLIMIT_MEMLOCK:
			rlp->rlim_max = 0x7FFFFFFFL;
			break;
		case RLIMIT_NOFILE:
			rlp->rlim_max = OPEN_MAX * 8;
			break;
		case RLIMIT_NPROC:
			rlp->rlim_max = CHILD_MAX * 8;
			break;
		case RLIMIT_FSIZE:	/* tar.exe */
		case RLIMIT_CORE:
			rlp->rlim_max = RLIM_INFINITY;
			break;
		default:
			result = -EINVAL;
	}
	return(result);
}
void 
rtime_posix(struct timeval *tp, DWORDLONG *Time)
{
	LONGLONG llTime = *Time;

	llTime *= 0.1;					/* microseconds */
	tp->tv_sec = (time_t)(llTime * 0.000001);
	tp->tv_usec = llTime - (tp->tv_sec * 1000000);
//__PRINTF("rtime_posix: %I64d.%d\n", tp->tv_sec, tp->tv_usec)
}
int 
getrusage_SELF(WIN_TASK *Task, struct rusage *usage)
{
	int result = 0;
	WIN_RUSAGE ruInfo = {0};

	if (!vfs_getrusage_SELF(Task->ThreadId, &ruInfo)){
		result -= errno_posix(GetLastError());
	}else{
		win_bzero(usage, sizeof(struct rusage));
		rtime_posix(&usage->ru_utime, (DWORDLONG *)&ruInfo.User);
		rtime_posix(&usage->ru_stime, (DWORDLONG *)&ruInfo.Kernel);
		if (Task->TracePoints & KTRFAC_STRUCT){
			ktrace_STRUCT(Task, "rusage", 6, usage, sizeof(struct rusage));
		}
	}
	return(result);
}
int 
getrusage_CHILDREN(WIN_TASK *Task, struct rusage *usage)
{
	int result = 0;
	WIN_RUSAGE ruInfo = {0};

	if (!vfs_getrusage_CHILDREN(Task->TaskId, &ruInfo)){
		result -= errno_posix(GetLastError());
	}else{
		win_bzero(usage, sizeof(struct rusage));
		rtime_posix(&usage->ru_utime, &ruInfo.User);
		rtime_posix(&usage->ru_stime, &ruInfo.Kernel);
		if (Task->TracePoints & KTRFAC_STRUCT){
			ktrace_STRUCT(Task, "rusage", 6, usage, sizeof(struct rusage));
		}
	}
	return(result);
}

/************************************************************/

int 
sys_getrusage(call_t call, int who, struct rusage *usage)
{
	int result = 0;

	if (!usage){
		result = -EFAULT;
	}else switch (who){
		case RUSAGE_SELF:
			result = getrusage_SELF(call.Task, usage);
			break;
		case RUSAGE_CHILDREN:
			result = getrusage_CHILDREN(call.Task, usage);
			break;
		default:
			result = -EINVAL;
	}
	return(result);
}
int 
sys_getrlimit(call_t call, int resource, struct rlimit *rlp)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (!rlp){
		result = -EFAULT;
	}else if (resource < 0 || resource >= RLIM_NLIMITS){
		result = -EINVAL;
	}else{
		result = rlimit_posix(pwTask, resource, rlp);
	}
	return(result);
}
int 
sys_setrlimit(call_t call, int resource, const struct rlimit *rlp)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (!rlp){
		result = -EFAULT;
	}else if (resource < 0 || resource >= RLIM_NLIMITS){
		result = -EINVAL;
	}else if (rlp->rlim_cur > rlp->rlim_max){
		result = -EINVAL;
	}else{
		pwTask->Limit[resource] = rlp->rlim_cur;
	}
	return(result);
}
int 
sys_getpriority(call_t call, int which, id_t who)
{
	int result = 0;

	if (who < 0 || who >= CHILD_MAX){
		result = -EINVAL;
	}else if (which == PRIO_PROCESS){
		result = __Tasks[who].Nice;
	}else{
__PRINTF("getpriority(%d): who(%d)\n", which, who)
		result = -EOPNOTSUPP;
	}
	return(result);
}
int 
sys_setpriority(call_t call, int which, id_t who, int value)
{
	int result = 0;

	if (who < 0 || who >= CHILD_MAX){
		result = -EINVAL;
	}else if (which == PRIO_PROCESS){
		__Tasks[who].Nice = value;
	}else{
__PRINTF("setpriority(%d): who(%d) value(%d)\n", which, who, value)
		result = -EOPNOTSUPP;
	}
	return(result);
}
