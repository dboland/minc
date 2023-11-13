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

//__PRINTF("origin(0x%lx) src(0x%lx) dest(0x%lx) __THREAD_FRAME(0x%lx)\n", 
//			origin, src, dest[0], __THREAD_FRAME)
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
//			depth, src, dest[0], *(long *)(src + 4), size)
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
	pwTask = proc_init(sigproc_win);
	srand(time_posix(&pwTask->Started));

	pszCommand = __Strings[pwTask->TaskId].Command;
	win_mbstowcs(pszCommand, argv[0], WIN_MAX_PROCTITLE);
	argv[0] = path_posix(cmdbuf, pszCommand);
//__PRINTF("title: %s\n", _PATH_BUF)

	/* in WinNT the groups are significant */
//	if (wStat.Mode.Special & WIN_S_ISGID){
//		task->ts_issetugid = 1;
//		win_setegid(pwTask, &wStat.GroupSid);
//	}
	if (win_group_member(&SidAdmins)){
		__RootUid = rid_posix(win_geteuid(&sid));
		__RootGid = rid_posix(win_getegid(&sid));
	}
	TlsSetValue(__TlsIndex, (PVOID)pwTask->TaskId);
}
void 
task_copy(WIN_THREAD_STRUCT *Thread)
{
//__PRINTF("PARENT: return(0x%lx) origin(0x%lx) size(0x%lx) source(0x%lx) dest(0x%lx) offset(0x%lx) Tls(0x%lx) Token(%lu) Flags(0x%x) di(0x%lx) si(0x%lx) bx(0x%lx) cx(0x%lx) dx(0x%lx) cs(0x%lx) ds(0x%lx) es(0x%lx) fs(0x%lx) gs(0x%lx) ss(0x%lx)\n", 
//		Parent->ret, Parent->origin, Parent->size, Parent->source, Parent->dest, Parent->offset, Parent->Tls, Parent->Token, Parent->Flags, Parent->di, Parent->si, Parent->bx, Parent->cx, Parent->dx, Parent->cs, Parent->ds, Parent->es, Parent->fs, Parent->gs, Parent->ss)
	/* copy frames */
	copy_stack(Thread->origin, Thread->source, &Thread->dest);
	/* add current frame */
	Thread->source -= Thread->offset;
	Thread->dest -= Thread->offset;
	memcpy((void *)Thread->dest, (void *)Thread->source, Thread->offset);
	/* copy thread local storage */
	copy_thread(Thread);
}
