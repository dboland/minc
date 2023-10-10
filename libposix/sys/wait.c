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

#include <sys/wait.h>

/************************************************************/

int 
taskv_GRP(DWORD GroupId, WIN_TASK *Result[])
{
	int index = WIN_PID_INIT;	/* init has 1 */
	WIN_TASK *pwTask = &__Tasks[index];
	int count = 0;

	while (index < CHILD_MAX){
		if (pwTask->GroupId == GroupId){
			Result[count++] = pwTask;
		}
		index++;
		pwTask++;
	}
	return(count);
}
int 
taskv_ANY(DWORD ParentId, WIN_TASK *Result[])
{
	int index = WIN_PID_INIT;	/* init has 1 */
	WIN_TASK *pwTask = &__Tasks[index];
	int count = 0;

	while (index < CHILD_MAX){
		if (pwTask->ParentId == ParentId){
			Result[count++] = pwTask;
		}
		index++;
		pwTask++;
	}
	return(count);
}
int 
taskv_PID(DWORD TaskId, WIN_TASK *Result[])
{
	int count = 0;
	WIN_TASK *pwTask = &__Tasks[TaskId];

	if (pwTask->Handle){
		Result[count++] = pwTask;
	}
	return(count);
}
int 
taskv_win(WIN_TASK *Parent, pid_t pid, WIN_TASK *Result[])
{
	int count = 0;

	if (pid == Parent->TaskId){	/* wait on self (perl.exe) */
		return(0);
	}else if (pid == WAIT_MYPGRP){	/* 0 */
		count = taskv_GRP(Parent->GroupId, Result);
	}else if (pid == WAIT_ANY){		/* -1 */
		count = taskv_ANY(Parent->TaskId, Result);
	}else if (pid > 0){
		count = taskv_PID(pid, Result);
	}else if (pid < -1){
		count = taskv_GRP(-pid, Result);
	}
	Result[count] = NULL;
	return(count);
}

/************************************************************/

pid_t 
sys_wait4(call_t call, pid_t pid, int *status, int options, struct rusage *rusage)
{
	pid_t result = 0;
	WIN_USAGE wuResult = {0};
	WIN_TASK *ptVector[CHILD_MAX + 1];
	BOOL bNoHang = FALSE;
	WIN_TASK *pwTask = call.Task;
	DWORD dwStatus = 0;

	if (options & WNOHANG){
		bNoHang = TRUE;
	}
	if (options & WUNTRACED){
		dwStatus = 0xFFFF;
	}
	if (pid >= CHILD_MAX){
		result = -EINVAL;
	}else if (!taskv_win(pwTask, pid, ptVector)){
		result = -ECHILD;
	}else if (!vfs_wait4(pwTask, ptVector, bNoHang, dwStatus, &wuResult)){
		result -= errno_posix(GetLastError());
	}else{
		result = wuResult.TaskId;
	}
	if (status){
		*status = wuResult.Status;
	}
	return(result);
}
