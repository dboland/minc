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

#include <sys/sched.h>

/****************************************************/

void 
copy_thread(WIN_THREAD_STRUCT *Thread)
{
	WIN_TASK *pwTask;

	pwTask = proc_dup(&__Tasks[Thread->TaskId], Thread);
	srand(time_posix(&pwTask->Started));
	Thread->Result = pwTask->TaskId;
//__PRINTF("copy_thread(%lu): task(%lu) pid(%d) ppid(%d)\n", 
//		Parent->Tls, result, result->pid, result->ppid)
	TlsSetValue(__TlsIndex, (PVOID)pwTask->TaskId);
}
int 
copy_stack(u_long origin, u_long src, u_long *dest)
{
	int depth;
	int size;
	u_long next;

//__PRINTF("+ origin(0x%lx) src(0x%lx) dest(0x%lx) __THREAD_FRAME(0x%lx)\n", 
//	origin, src, dest[0], __THREAD_FRAME)
	if (!src){				/* at process frame (_start()) */
		depth = 0;
	}else if (src == __THREAD_FRAME){	/* at thread frame (main()) */
		depth = 0;
	}else{
		next = *(u_long *)src;
		depth = copy_stack(origin, next, dest);
		if (!next){
			size = origin - src;
		}else{
			size = next - src;
		}
//__PRINTF("+ copy_stack(%d): source(0x%lx) dest(0x%lx) ret(0x%lx) size(%d)\n", 
//	depth, src, dest[0], *(long *)(src + 4), size)
		dest[0] -= size;
		memcpy((void *)dest[0], (void *)src, size);
		if (!depth){
			*(u_long *)(dest[0] + sizeof(u_long)) = (u_long)&__threxit;
		}
		*(u_long *)dest[0] = dest[0] + size;
		depth++;
	}
	return(depth);
}

/****************************************************/

void 
task_init(char *cmdbuf, char *argv[], void *frame_address)
{
	int fd = 0;
	SID8 sid;
	WIN_TASK *pwTask;
	LPWSTR pszCommand;

	__THREAD_FRAME = (u_long)frame_address;
	pwTask = proc_start(sigproc_win);
	srand(time_posix(&pwTask->Started));
	if (pwTask->Flags & WIN_PS_SYSTEM){			/* top.exe */
		pwTask->RealUid = rid_posix(&pwTask->UserSid);
		pwTask->RealGid = rid_posix(&pwTask->GroupSid);
	}
	pszCommand = __Strings[pwTask->TaskId].Command;
	win_mbstowcs(pszCommand, argv[0], WIN_MAX_PROCTITLE);
	argv[0] = path_posix(cmdbuf, pszCommand);

	TlsSetValue(__TlsIndex, (PVOID)pwTask->TaskId);
}
void 
task_copy(WIN_THREAD_STRUCT *Thread)
{
//vfs_ktrace("task_copy", STRUCT_THREAD, Thread);
	/* copy frames */
	copy_stack(Thread->origin, Thread->source, &Thread->dest);
	/* add current frame */
	Thread->source -= Thread->offset;
	Thread->dest -= Thread->offset;
	memcpy((void *)Thread->dest, (void *)Thread->source, Thread->offset);
	/* copy thread control block */
	copy_thread(Thread);
}
