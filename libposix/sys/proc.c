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

#include <sys/proc.h>

/****************************************************/

DWORD 
pid_win(pid_t pid)
{
	DWORD dwResult = 0;

	if (pid < 0 || pid >= CHILD_MAX){
		SetLastError(ERROR_BAD_ARGUMENTS);
	}else{
		dwResult = __Tasks[pid].ThreadId;
	}
	return(dwResult);
}
u_int32_t 
rtime_posix(DWORDLONG *Started)
{
	DWORDLONG dwlCurrent;
	DWORDLONG dwlRealTime;

	GetSystemTimeAsFileTime((LPFILETIME)&dwlCurrent);
	dwlRealTime = dwlCurrent - *Started;		/* 100-nanosecond intervals */
	dwlRealTime *= 0.0000001;			/* seconds */
	return((u_int32_t)dwlRealTime);
}
void 
kargv_posix(pid_t pid, int argc, char *argv[])
{
	char *buf = (char *)(argv + argc + 1);

	*argv++ = buf;
	*argv = NULL;
	pathn_posix(buf, __Strings[pid].Command, WIN_MAX_PROCTITLE);
}
struct kinfo_proc *
kproc_posix(WIN_TASK *Task, struct kinfo_proc *proc)
{
	WIN_TERMIO *pTerminal = &__Terminals[Task->TerminalId];
	wchar_t *command = win_basename(__Strings[Task->TaskId].Command);
	struct timeval tv;
	WIN_KINFO_PROC kInfo = {0};
	DWORD dwPageSize = __Globals[WIN_HW_PAGESIZE].LowPart;

	/* see: src/sys/sys/sysctl.h */

	win_bzero(proc, sizeof(struct kinfo_proc));
	win_wcstombs(proc->p_comm, command, KI_MAXCOMLEN);
//	proc->p_flag					/* INT: P_* flags. */
	proc->p_psflags = Task->Flags;		/* INT: PS_* flags on the process. */
	proc->p_pid = Task->TaskId;
	proc->p_ppid = Task->ParentId;
	proc->p_sid = Task->SessionId;
	proc->p__pgid = Task->GroupId;
	proc->p_uid = rid_posix(&Task->UserSid);
	proc->p_ruid = Task->RealUid;
	proc->p_svuid = Task->SavedUid;
	if (proc->p_uid == WIN_ROOT_UID){
		proc->p_uid = 0;
	}
	proc->p_gid = rid_posix(&Task->GroupSid);
	proc->p_rgid = Task->RealGid;
	proc->p_svgid = Task->SavedGid;
	if (proc->p_gid == WIN_ROOT_GID){
		proc->p_gid = 0;
	}
	proc->p_tdev = pTerminal->DeviceId;
	proc->p_tpgid = pTerminal->GroupId;
	proc->p_vm_rssize = 10;
	proc->p_vm_tsize = 10;
	proc->p_vm_dsize = 10;
	proc->p_vm_ssize = (WIN_STACKSIZE + dwPageSize - 1) / dwPageSize;
	if (win_KERN_PROC(Task->ThreadId, &kInfo)){
		proc->p_uvalid = 1;			/* CHAR: following p_u* members from struct user are valid */
		proc->p_ustart_sec = time_posix(&kInfo.Created);
		proc->p_uutime_sec = time_posix(&kInfo.User);
		proc->p_ustime_sec = time_posix(&kInfo.Kernel);
	}else{
		proc->p_ustart_sec = time_posix(&Task->Started);
	}
	proc->p_stat = Task->State;			/* CHAR: S* process status (from LWP). */
	proc->p_cpuid = KI_NOCPU;			/* LONG: CPU id */
	proc->p_nice = Task->Nice;
	proc->p_xstat = Task->Status >> 8;		/* U_SHORT: Exit status for wait; also stop signal. */
	proc->p_tracep = (u_int32_t)Task->TraceHandle;
	proc->p_rtime_sec = rtime_posix((DWORDLONG *)&Task->Started);
	if (proc->p_stat == SSLEEP){
		win_strncpy(proc->p_wmesg, syscallnames[Task->Code], KI_WMESGLEN);
	}
	proc->p_tid = Task->ThreadId;
	return(proc + 1);
}

/****************************************************/

struct kinfo_proc *
proc_KERN_PROC_ALL(WIN_TASK *Task, struct kinfo_proc *buf, size_t *size)
{
	if (Task->Flags){
		if (!buf){
			*size += sizeof(struct kinfo_proc);
		}else{
			buf = kproc_posix(Task, buf);
		}
	}
	return(buf);
}
struct kinfo_proc *
proc_KERN_PROC_PGRP(WIN_TASK *Task, pid_t pgid, struct kinfo_proc *buf, size_t *size)
{
	if (Task->GroupId == pgid){
		if (!buf){
			*size += sizeof(struct kinfo_proc);
		}else{
			buf = kproc_posix(Task, buf);
		}
	}
	return(buf);
}
struct kinfo_proc *
proc_KERN_PROC_UID(WIN_TASK *Task, uid_t uid, struct kinfo_proc *buf, size_t *size)
{
	uid_t rid = rid_posix(&Task->UserSid);

	if (!uid){
		uid = WIN_ROOT_UID;
	}
	if (rid == uid){
		if (!buf){
			*size += sizeof(struct kinfo_proc);
		}else{
			buf = kproc_posix(Task, buf);
		}
	}
	return(buf);
}
struct kinfo_proc *
proc_KERN_PROC_KTHREAD(WIN_TASK *Task, int tid, struct kinfo_proc *buf, size_t *size)
{
	if (Task->ThreadId == tid){
		if (!buf){
			*size += sizeof(struct kinfo_proc);
		}else{
			buf = kproc_posix(Task, buf);
		}
	}
	return(buf);
}
