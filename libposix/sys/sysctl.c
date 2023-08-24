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

#include <uvm/uvm_param.h>		/* VM_LOADAVG */
#include <uvm/uvm_extern.h>	/* struct uvmexp */

/****************************************************/

void 
sysctl_debug(const int *name, u_int namelen, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	msvc_printf("sysctl(");
	while (namelen--){
		msvc_printf("[%d]", *name);
		name++;
	}
	msvc_printf("): oldp(0x%x) oldlen(%d) newp(0x%x) newlen(%d)\n", 
		oldp, *oldlenp, newp, newlen);
}

/****************************************************/

int 
sysctl_KERN_CLOCKRATE(WIN_TASK *Task, struct clockinfo *info)
{
	int result = -1;
	DWORDLONG dwlHertz, dwlTick;

	if (!win_KERN_CLOCKRATE(&dwlHertz)){
		__errno_posix(Task, GetLastError());
	}else if (!win_KERN_TIMECOUNTER_TICK(&dwlTick)){
		__errno_posix(Task, GetLastError());
	}else{
		info->hz = dwlHertz;
		info->tick = dwlTick;
		info->tickadj = 0;
		info->stathz = dwlHertz;
		info->profhz = dwlHertz;
		result = 0;
	}
	return(result);
}
int 
sysctl_KERN_HOSTNAME(WIN_TASK *Task, char *curname, size_t *csize, const char *newname, size_t nsize)
{
	int result = -1;
	size_t size = 0;

	if (csize){
		size = *csize;
	}
	/* size not needed for new name */
	if (!win_KERN_HOSTNAME(curname, newname, size)){
		__errno_posix(Task, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
int 
sysctl_KERN_DOMAINNAME(WIN_TASK *Task, char *curname, size_t *csize, const char *newname, size_t nsize)
{
	int result = -1;
	size_t size = 0;

	if (csize){
		size = *csize;
	}
	/* size not needed for new name */
	if (!win_KERN_DOMAINNAME(curname, newname, size)){
		__errno_posix(Task, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
int 
sysctl_KERN_PROC(const int *name, void *data, size_t *size)
{
	int result = 0;
	pid_t pid = WIN_PID_INIT;
	WIN_TASK *pwTask = &__Tasks[pid];

	/* sys/proc.c */

	while (pid < CHILD_MAX){
		if (pwTask->Flags){
			if (!data){
				*size += sizeof(struct kinfo_proc);
			}else{
				data = kproc_posix(pwTask, data);
			}
		}
		pid++;
		pwTask++;
	}
	return(result);
}
int 
sysctl_KERN_PROC_NEW(const int *name, void *data, size_t *size)
{
	int result = 0;
	pid_t pid = WIN_PID_INIT;
	WIN_TASK *pwTask = &__Tasks[pid];

	/* sys/proc.c */

	while (pid < CHILD_MAX){
		switch (name[2]){
			case KERN_PROC_KTHREAD:		/* kvm_getprocs(3): user-level plus kernel threads */
			case KERN_PROC_ALL:
				data = proc_KERN_PROC_ALL(pwTask, (struct kinfo_proc *)data, size);
				break;
			case KERN_PROC_PGRP:
				data = proc_KERN_PROC_PGRP(pwTask, name[3], (struct kinfo_proc *)data, size);
				break;
			case KERN_PROC_UID:
				data = proc_KERN_PROC_UID(pwTask, name[3], (struct kinfo_proc *)data, size);
				break;
			default:
//				__errno_posix(Task, ERROR_NOT_SUPPORTED);
				return(-1);
		}
		pid++;
		pwTask++;
	}
	return(result);
}
int 
sysctl_KERN_PROC_ARGS(WIN_TASK *Task, const int *name, void *buf, size_t *size)
{
	int result = 0;
	pid_t pid = name[2];

	/* sys/proc.c */

//__PRINTF("PROC_ARGV(%d): buf(0x%x) size(%d)\n", pid, buf, *size)
	switch (name[3]){
		case KERN_PROC_ARGV:
			kargv_posix(pid, 1, buf);
			break;
		case KERN_PROC_NARGV:
		case KERN_PROC_ENV:
		case KERN_PROC_NENV:
		default:
			result = -1;
	}
	return(result);
}
int 
sysctl_KERN_CPTIME(WIN_TASK *Task, long states[CPUSTATES])
{
	win_bzero(states, sizeof(long) * CPUSTATES);
//	states[2] = GetTickCount();
	return(0);
}
int 
sysctl_KERN_CPTIME2(WIN_TASK *Task, int cpu, u_int64_t states[CPUSTATES])
{
	win_bzero(states, sizeof(u_int64_t) * CPUSTATES);
//	states[2] = GetTickCount();
	return(0);
}
int 
sysctl_KERN_VERSION(WIN_TASK *Task, char *buf, size_t bufsize)
{
	int size = 0;

	size += msvc_sprintf(buf, "OpenBSD %s (MINC): #%s: ", "6.1", VERSION);
	size += msvc_strtime(__Globals[WIN_KERN_BUILD].LowPart, buf + size, bufsize - size);
	return(0);
}
int 
sysctl_KERN_SECURELVL(WIN_TASK *Task, int *oldvalue, int *newvalue)
{
	int result = 0;

	if (oldvalue){
		*oldvalue = __Globals[WIN_KERN_SECURELVL].LowPart;
	}else if (newvalue){
		__Globals[WIN_KERN_SECURELVL].LowPart = *newvalue;
	}else{
		__errno_posix(Task, ERROR_BAD_ARGUMENTS);
		result = -1;
	}
	return(result);
}
int 
sysctl_KERN_TIMECOUNTER_TICK(WIN_TASK *Task, int *value)
{
	int result = -1;
	DWORDLONG dwlValue;

	if (!win_KERN_TIMECOUNTER_TICK(&dwlValue)){
		__errno_posix(Task, ERROR_INVALID_NAME);
	}else{
		*value = (int)dwlValue;
		result = 0;
	}
	return(result);
}
int 
sysctl_KERN_TIMECOUNTER(WIN_TASK *Task, const int *name, void *oldvalue, void *newvalue)
{
	int result = -1;

	switch (name[2]){
		case KERN_TIMECOUNTER_TICK:
			result = sysctl_KERN_TIMECOUNTER_TICK(Task, (int *)oldvalue);
			break;
		default:
			__errno_posix(Task, ERROR_INVALID_NAME);
	}
	return(result);
}
int 
sysctl_KERN(WIN_TASK *Task, const int *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = 0;

	switch (name[1]){
		case KERN_CLOCKRATE:		/* mcount() */
			result = sysctl_KERN_CLOCKRATE(Task, (struct clockinfo *)oldp);
			break;
		case KERN_RAWPARTITION:	/* GNU conftest.exe */
			*(int *)oldp = 0;
			break;
		case KERN_MAXPARTITIONS:
			*(int *)oldp = MAXPARTITIONS;
			break;
		case KERN_SAVED_IDS:		/* vim.exe */
			*(int *)oldp = 1;
			break;
		case KERN_JOB_CONTROL:
			*(int *)oldp = 1;
			break;
		case KERN_MAXVNODES:
			*(int *)oldp = OPEN_MAX;
			break;
		case KERN_HOSTNAME:
			result = sysctl_KERN_HOSTNAME(Task, (char *)oldp, oldlenp, (const char *)newp, newlen);
			break;
		case KERN_DOMAINNAME:
			result = sysctl_KERN_DOMAINNAME(Task, (char *)oldp, oldlenp, (const char *)newp, newlen);
			break;
		case KERN_OSTYPE:
			win_strncpy(oldp, "OpenBSD", *oldlenp);
			break;
		case KERN_OSRELEASE:
			win_strncpy(oldp, RELEASE, *oldlenp);
			break;
		case KERN_OSVERSION:
			win_strncpy(win_stpcpy(oldp, "MINC#"), VERSION, *oldlenp - 5);
			break;
		case KERN_ARGMAX:
			*(int *)oldp = MAX_ARGBUF - (MAX_ARGBUF % MIN_BUFSIZE);
			break;
		case KERN_MSGBUFSIZE:
			result = msgbuf_KERN_MSGBUFSIZE((int *)oldp, oldlenp);
			break;
		case KERN_MSGBUF:
			result = msgbuf_KERN_MSGBUF((char *)oldp, oldlenp);
			break;
		case KERN_PROC:
			result = sysctl_KERN_PROC(name, oldp, oldlenp);
			break;
		case KERN_PROC_ARGS:
			result = sysctl_KERN_PROC_ARGS(Task, name, oldp, oldlenp);
			break;
		case KERN_FSCALE:	/* The kernel fixed-point scale factor (ps.exe) */
			*(int *)oldp = FSCALE;
			break;
		case KERN_CCPU:	/* The scheduler exponential decay value (ps.exe) */
			*(int *)oldp = 1948;
			break;
		case KERN_CPTIME:	/* the number of ticks spent by the system (top.exe) */
			result = sysctl_KERN_CPTIME(Task, (long *)oldp);
			break;
		case KERN_CPTIME2:	/* ticks per CPU (top.exe) */
			result = sysctl_KERN_CPTIME2(Task, name[2], (u_int64_t *)oldp);
			break;
		case KERN_VERSION:
			result = sysctl_KERN_VERSION(Task, oldp, *oldlenp);
			break;
		case KERN_SECURELVL:
			result = sysctl_KERN_SECURELVL(Task, (int *)oldp, (int *)newp);
			break;
		case KERN_TIMECOUNTER:
			result = sysctl_KERN_TIMECOUNTER(Task, name, oldp, newp);
			break;
		case KERN_TTYCOUNT:
			*(int *)oldp = WIN_TTY_MAX;
			break;
//		case KERN_ALLOWKMEM:
//			*(int *)oldp = 0;
//			break;
		case KERN_FILE:		/* fstat.exe */
			result = file_KERN_FILE(Task, name, oldp, oldlenp);
			break;
		case KERN_HOSTID:	/* alpine.exe */
		default:
			__errno_posix(Task, ERROR_INVALID_NAME);
			result = -1;
	}
	return(result);
}

/****************************************************/

int 
sysctl_HW_PHYSMEM(int *value)
{
	MEMORYSTATUS msInfo = {sizeof(MEMORYSTATUS), 0};

	GlobalMemoryStatus(&msInfo);
	*value = msInfo.dwTotalPhys;
	return(0);
}
int 
sysctl_HW_PHYSMEM64(quad_t *value)
{
	MEMORYSTATUSEX msInfo = {sizeof(MEMORYSTATUSEX), 0};

	GlobalMemoryStatusEx(&msInfo);
	*value = msInfo.ullTotalPhys;
	return(0);
}
int 
sysctl_HW_USERMEM(int *value)
{
	SYSTEM_INFO sInfo;

	GetSystemInfo(&sInfo);
	*value = sInfo.lpMaximumApplicationAddress - sInfo.lpMinimumApplicationAddress;
	return(0);
}
int 
sysctl_HW_NCPU(int *value)
{
	SYSTEM_INFO sInfo;

	GetSystemInfo(&sInfo);
	*value = sInfo.dwNumberOfProcessors;
	return(0);
}
int 
sysctl_HW(WIN_TASK *Task, const int *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = -1;

	switch (name[1]){
		case HW_MACHINE:
			win_strncpy(oldp, MACHINE, *oldlenp);
			result = 0;
			break;
		case HW_PAGESIZE:
			*(int *)oldp = __Globals[WIN_HW_PAGESIZE].LowPart;
			result = 0;
			break;
		case HW_PHYSMEM:	/* gcc.exe/collect2.exe */
			result = sysctl_HW_PHYSMEM((int *)oldp);
			break;
		case HW_PHYSMEM64:
			result = sysctl_HW_PHYSMEM64((quad_t *)oldp);
			break;
		case HW_USERMEM:
			result = sysctl_HW_USERMEM((int *)oldp);
			break;
		case HW_NCPU:
			result = sysctl_HW_NCPU((int *)oldp);
			break;
		case HW_DISKNAMES:
			win_strncpy(oldp, "wd0:,cd0:,sd0:", *oldlenp);
			result = 0;
			break;
		default:
			__errno_posix(Task, ERROR_INVALID_NAME);
	}
	return(result);
}

/****************************************************/

int 
sysctl_VM_LOADAVG(WIN_TASK *Task, struct loadavg *load, size_t size)
{
	int result = -1;
	DWORD dwCount = size / sizeof(struct loadavg);

	if (!proc_VM_LOADAVG(__Tasks, (WIN_LOADAVG *)load)){
		__errno_posix(Task, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
int 
sysctl_VM_UVMEXP(WIN_TASK *Task, struct uvmexp *uvm, size_t size)
{
	int result = -1;
	MEMORYSTATUSEX msInfo = {sizeof(MEMORYSTATUSEX), 0};
	SYSTEM_INFO sInfo;
	DWORD dwPageSize;
	DWORDLONG dwlTotal, dwlAvail, dwlUsed;

	win_bzero(uvm, size);
	GetSystemInfo(&sInfo);
	dwPageSize = sInfo.dwPageSize;
	GlobalMemoryStatusEx(&msInfo);

	uvm->pagemask = sInfo.dwActiveProcessorMask;

	dwlTotal = msInfo.ullTotalPhys;
	dwlAvail = msInfo.ullAvailPhys;
	dwlUsed = dwlTotal - dwlAvail;

//__PRINTF("Phys: dwAvail(%I64d) dwTotal(%I64d) dwUsed(%I64d)\n", dwAvail, dwTotal, dwUsed)

	uvm->active = (dwlUsed + dwPageSize - 1) / dwPageSize;
	uvm->inactive = (dwlAvail + dwPageSize - 1) / dwPageSize;

	dwlTotal = msInfo.ullTotalPageFile;
	dwlAvail = msInfo.ullAvailPageFile;
	dwlUsed = dwlTotal - dwlAvail;

	uvm->nswapdev = 1;
	uvm->swpages = (dwlTotal + dwPageSize - 1) / dwPageSize;
	uvm->swpginuse = (dwlUsed + dwPageSize - 1) / dwPageSize;
	uvm->swpgonly = uvm->swpginuse;

	dwlTotal = msInfo.ullTotalVirtual;
	dwlAvail = msInfo.ullAvailVirtual;
	dwlUsed = dwlTotal - dwlAvail;

	uvm->pagesize = dwPageSize;
	uvm->npages = (dwlTotal + dwPageSize - 1) / dwPageSize;
	uvm->free = (dwlAvail + dwPageSize - 1) / dwPageSize;

	return(0);
}
int 
sysctl_VM(WIN_TASK *Task, const int *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = 0;

	switch (name[1]){
		case VM_LOADAVG:
			result = sysctl_VM_LOADAVG(Task, (struct loadavg *)oldp, *oldlenp);
			break;
		case VM_MAXSLP:	/* time to be blocked before being swappable (ps.exe) */
			*(int *)oldp = 0;
			break;
		case VM_UVMEXP:	/* statistics about the UVM memory management system (top.exe) */
			result = sysctl_VM_UVMEXP(Task, (struct uvmexp *)oldp, *oldlenp);
			break;
		default:
			__errno_posix(Task, ERROR_INVALID_NAME);
			result = -1;
	}
	return(result);
}

/****************************************************/

int 
sysctl_NET_RT(WIN_TASK *Task, const int *name, void *buf, size_t *size)
{
	int result = -1;

	/* net/route.c */

	switch (name[4]){
		case NET_RT_DUMP:	/* 1 */
			result = route_NET_RT_DUMP(Task, buf, size);
			break;
		case NET_RT_FLAGS:	/* 2 */
			result = route_NET_RT_FLAGS(Task, name, buf, size);
			break;
		case NET_RT_IFLIST:	/* 3 */
			result = route_NET_RT_IFLIST(Task, buf, size);
			break;
		default:
			__errno_posix(Task, ERROR_INVALID_NAME);
	}
	return(result);
}
int 
sysctl_NET_INET(WIN_TASK *Task, const int *name, void *buf, size_t *size)
{
	int result = -1;

	switch (name[2]){
		case IPPROTO_IP:	/* netinet/ip.c */
			result = ip_NET_INET_IP(Task, name, buf, size);
			break;
		case IPPROTO_TCP:	/* netinet/tcp.c */
			result = tcp_NET_INET_TCP(Task, name, buf, size);
			break;
		case IPPROTO_UDP:	/* netinet/udp.c */
			result = udp_NET_INET_UDP(Task, name, buf, size);
			break;
		default:
			__errno_posix(Task, ERROR_INVALID_NAME);
	}
	return(result);
}
int 
sysctl_NET_INET6(WIN_TASK *Task, const int *name, void *buf, size_t *size)
{
	int result = -1;

	/* netinet/in.c */

	switch (name[2]){
		case IPPROTO_IPV6:	/* 41 */
			result = in_NET_INET6_IPV6(Task, name, buf, size);
			break;
		default:
			__errno_posix(Task, ERROR_INVALID_NAME);
	}
	return(result);
}
int 
sysctl_NET(WIN_TASK *Task, const int *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = -1;

	switch (name[1]){
		case PF_ROUTE:
			result = sysctl_NET_RT(Task, name, oldp, oldlenp);
			break;
		case PF_INET:
			result = sysctl_NET_INET(Task, name, oldp, oldlenp);
			break;
		case PF_INET6:
			result = sysctl_NET_INET6(Task, name, oldp, oldlenp);
			break;
		case PF_KEY:
			*oldlenp = 0;		/* no IPSEC (ifconfig.exe) */
			result = 0;
			break;
		default:
			__errno_posix(Task, ERROR_INVALID_NAME);
	}
	return(result);
}

/****************************************************/

int 
sysctl_VFS_BCACHESTAT(struct bcachestats *bcstats, size_t size)
{
	win_bzero(bcstats, size);
	return(0);
}
int 
sysctl_VFS_GENERIC(WIN_TASK *Task, const int *name, void *buf, size_t *size)
{
	int result = 0;

	switch (name[2]){
		case VFS_BCACHESTAT:		/* nbytes in VNODE cache (top.exe) */
			result = sysctl_VFS_BCACHESTAT((struct bcachestats *)buf, *size);
			break;
		default:
			__errno_posix(Task, ERROR_INVALID_NAME);
			result = -1;
	}
	return(result);
}
int 
sysctl_VFS(WIN_TASK *Task, const int *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = -1;

	switch (name[1]){
		case VFS_GENERIC:
			result = sysctl_VFS_GENERIC(Task, name, oldp, oldlenp);
			break;
		default:
			__errno_posix(Task, ERROR_INVALID_NAME);
	}
	return(result);
}

/****************************************************/

int 
sysctl_MACHDEP_CPU_BIOS(WIN_TASK *Task, const int *name, char **buf, size_t *size)
{
	int result = -1;

	switch (name[2]){
		case BIOS_DEV:
			*(dev_t *)buf = DEV_TYPE_PORT;
			result = 0;
			break;
		case BIOS_DISKINFO:
			__errno_posix(Task, ERROR_DEVICE_NOT_PARTITIONED);
			break;
		case BIOS_CKSUMLEN:
		case BIOS_MAXID:
		default:
			__errno_posix(Task, ERROR_INVALID_NAME);
	}
	return(result);
}
int 
sysctl_MACHDEP(WIN_TASK *Task, const int *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = 0;

	switch (name[1]){
		case CPU_CONSDEV:
			*(dev_t *)oldp = DEV_TYPE_CONSOLE;
			break;
		case CPU_BIOS:
			result = sysctl_MACHDEP_CPU_BIOS(Task, name, (char **)oldp, oldlenp);
			break;
		default:
			__errno_posix(Task, ERROR_INVALID_NAME);
			result = -1;
	}
	return(result);
}

/****************************************************/

int 
sys___sysctl(call_t call, const int *name, u_int namelen, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = -1;
	WIN_TASK *pwTask = call.Task;

	if (namelen < 2 || namelen >= CTL_MAXNAME){
		__errno_posix(pwTask, ERROR_BAD_ARGUMENTS);
	}else switch(name[0]){
		case CTL_KERN:
			result = sysctl_KERN(pwTask, name, oldp, oldlenp, newp, newlen);
			break;
		case CTL_HW:
			result = sysctl_HW(pwTask, name, oldp, oldlenp, newp, newlen);
			break;
		case CTL_VM:
			result = sysctl_VM(pwTask, name, oldp, oldlenp, newp, newlen);
			break;
		case CTL_NET:
//sysctl_debug(name, namelen, oldp, oldlenp, newp, newlen);
			result = sysctl_NET(pwTask, name, oldp, oldlenp, newp, newlen);
			break;
		case CTL_VFS:
			result = sysctl_VFS(pwTask, name, oldp, oldlenp, newp, newlen);
			break;
		case CTL_MACHDEP:
			result = sysctl_MACHDEP(pwTask, name, oldp, oldlenp, newp, newlen);
			break;
		default:
			__errno_posix(pwTask, ERROR_INVALID_NAME);
	}
	return(result);
}
