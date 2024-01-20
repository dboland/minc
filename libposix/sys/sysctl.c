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
long 
ticks_posix(FILETIME *Time)
{
	LONGLONG llTime = *(LONGLONG *)Time;

	llTime -= 116444736000000000LL;	/* epoch */
	llTime *= 0.0000001;		/* milliseconds */
	return(llTime);
}
u_int64_t
ticks64_posix(LARGE_INTEGER *Time)
{
	LONGLONG llTime = Time->QuadPart;

	llTime -= 116444736000000000LL;	/* epoch */
	llTime *= 0.0000001;		/* milliseconds */
	return(llTime);
}

/****************************************************/

int 
sysctl_KERN_CLOCKRATE(struct clockinfo *info)
{
	info->hz = win_KERN_CLOCKRATE();
	info->tick = 1000000000 / info->hz;
	info->tickadj = 0;
	info->stathz = info->hz;
	info->profhz = info->hz;
	return(0);
}
int 
sysctl_KERN_HOSTNAME(char *curname, size_t *csize, const char *newname, size_t nsize)
{
	int result = 0;
	size_t size = 0;

	if (csize){
		size = *csize;
	}
	if (!win_KERN_HOSTNAME(curname, newname, size)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sysctl_KERN_DOMAINNAME(char *curname, size_t *csize, const char *newname, size_t nsize)
{
	int result = 0;
	size_t size = 0;

	if (csize){
		size = *csize;
	}
	if (!win_KERN_DOMAINNAME(curname, newname, size)){
		result -= errno_posix(GetLastError());
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
				data = kproc_posix(data, pwTask);
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
				return(-EINVAL);
		}
		pid++;
		pwTask++;
	}
	return(result);
}
int 
sysctl_KERN_PROC_ARGS(const int *name, void *buf, size_t *size)
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
			result = -ENOENT;
	}
	return(result);
}
int 
sysctl_KERN_CPTIME(long states[CPUSTATES])
{
	FILETIME ftIdle, ftKernel, ftUser;

	GetSystemTimes(&ftIdle, &ftKernel, &ftUser);
	states[CP_USER] = ticks_posix(&ftUser);
	states[CP_NICE] = 0;
	states[CP_SYS] = ticks_posix(&ftKernel);
	states[CP_INTR] = 0;
	states[CP_IDLE] = ticks_posix(&ftIdle);
	return(0);
}
int 
sysctl_KERN_CPTIME2(int cpu, u_int64_t states[CPUSTATES])
{
	int result = 0;
	LONG lCount = win_HW_NCPU();
	ULONG ulSize = sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * lCount;
	SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION *sppInfo = win_malloc(ulSize);

	if (cpu < 0 || cpu >= lCount){
		result = -EINVAL;
	}else if (!win_KERN_CPTIME2(sppInfo, ulSize)){
		result -= errno_posix(GetLastError());
	}else{
		states[CP_USER] = ticks64_posix(&sppInfo[cpu].UserTime);
		states[CP_NICE] = ticks64_posix(&sppInfo[cpu].DpcTime);
		states[CP_SYS] = ticks64_posix(&sppInfo[cpu].KernelTime);
		states[CP_INTR] = ticks64_posix(&sppInfo[cpu].InterruptTime);
		states[CP_IDLE] = ticks64_posix(&sppInfo[cpu].IdleTime);
	}
	win_free(sppInfo);
	return(result);
}
int 
sysctl_KERN_VERSION(char *buf, size_t bufsize)
{
	int size = 0;

	size += msvc_sprintf(buf, "OpenBSD %s (MINC): #%s: ", "6.1", VERSION);
	size += msvc_strtime(BUILD, buf + size, bufsize - size);
	return(0);
}
int 
sysctl_KERN_SECURELVL(int *oldvalue, int *newvalue)
{
	int result = 0;

	if (oldvalue){
		*oldvalue = __Globals[WIN_KERN_SECURELVL].LowPart;
	}else if (newvalue){
		__Globals[WIN_KERN_SECURELVL].LowPart = *newvalue;
	}else{
		result = -EINVAL;
	}
	return(result);
}
int 
sysctl_KERN(const int *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = 0;

	switch (name[1]){
		case KERN_CLOCKRATE:		/* mcount() */
			result = sysctl_KERN_CLOCKRATE((struct clockinfo *)oldp);
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
			result = sysctl_KERN_HOSTNAME((char *)oldp, oldlenp, (const char *)newp, newlen);
			break;
		case KERN_DOMAINNAME:
			result = sysctl_KERN_DOMAINNAME((char *)oldp, oldlenp, (const char *)newp, newlen);
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
			result = sysctl_KERN_PROC_ARGS(name, oldp, oldlenp);
			break;
		case KERN_FSCALE:	/* The kernel fixed-point scale factor (ps.exe) */
			*(int *)oldp = FSCALE;
			break;
		case KERN_CCPU:	/* The scheduler exponential decay value (ps.exe) */
			*(int *)oldp = 1948;
			break;
		case KERN_CPTIME:	/* the number of ticks spent by the system (top.exe) */
			result = sysctl_KERN_CPTIME((long *)oldp);
			break;
		case KERN_CPTIME2:	/* ticks per CPU (top.exe) */
			result = sysctl_KERN_CPTIME2(name[2], (u_int64_t *)oldp);
			break;
		case KERN_VERSION:
			result = sysctl_KERN_VERSION(oldp, *oldlenp);
			break;
		case KERN_SECURELVL:
			result = sysctl_KERN_SECURELVL((int *)oldp, (int *)newp);
			break;
		case KERN_TTYCOUNT:
			*(int *)oldp = WIN_TTY_MAX;
			break;
//		case KERN_ALLOWKMEM:
//			*(int *)oldp = 0;
//			break;
		case KERN_FILE:		/* fstat.exe */
			result = file_KERN_FILE(name, oldp, oldlenp);
			break;
		case KERN_HOSTID:	/* alpine.exe */
		default:
			result = -ENOENT;
	}
	return(result);
}

/****************************************************/

void 
sysctl_HW_DISKNAMES(char *buf, size_t size)
{
	WIN_DEVICE *pwDevice = DEVICE(DEV_CLASS_STORAGE);
	int unit = 0;
	char name[MAX_NAME];
	char *sep = "";
	int len;

	while (unit < WIN_UNIT_MAX){
		if (pwDevice->Flags){
			if (size < MAX_NAME){
				break;
			}else if (disk_HW_DISKNAMES(pwDevice, name)){
				len = msvc_sprintf(buf, "%s%s", sep, name);
				buf += len;
				size -= len;
				sep = ",";
			}
		}
		unit++;
		pwDevice++;
	}
}
int 
sysctl_HW(const int *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = 0;

	switch (name[1]){
		case HW_MACHINE:
			win_strncpy(oldp, MACHINE, *oldlenp);
			break;
		case HW_PAGESIZE:
			*(int *)oldp = win_HW_PAGESIZE();
			break;
		case HW_PHYSMEM:	/* gcc (collect2.exe) */
			*(int *)oldp = win_HW_PHYSMEM();
			break;
		case HW_PHYSMEM64:
			*(quad_t *)oldp = win_HW_PHYSMEM64();
			break;
		case HW_USERMEM:
			*(int *)oldp = win_HW_USERMEM();
			break;
		case HW_NCPU:
			*(int *)oldp = win_HW_NCPU();
			break;
		case HW_DISKNAMES:
			sysctl_HW_DISKNAMES((char *)oldp, *oldlenp);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}

/****************************************************/

int 
sysctl_VM_LOADAVG(struct loadavg *load, size_t size)
{
	int result = 0;
	DWORD dwCount = size / sizeof(struct loadavg);

	if (!proc_VM_LOADAVG(__Tasks, (WIN_LOADAVG *)load)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sysctl_VM_UVMEXP(struct uvmexp *uvm, size_t size)
{
	MEMORYSTATUSEX msInfo = {sizeof(MEMORYSTATUSEX), 0};
	DWORD dwPageSize;
	DWORDLONG dwlTotal, dwlAvail, dwlUsed;
	SYSTEM_INFO sInfo;

	win_bzero(uvm, size);
	GetSystemInfo(&sInfo);
	GlobalMemoryStatusEx(&msInfo);
	dwPageSize = sInfo.dwPageSize;

	uvm->pagemask = sInfo.dwActiveProcessorMask;

	dwlTotal = msInfo.ullTotalPhys;
	dwlAvail = msInfo.ullAvailPhys;
	dwlUsed = dwlTotal - dwlAvail;

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
sysctl_VM(const int *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = 0;

	switch (name[1]){
		case VM_LOADAVG:
			result = sysctl_VM_LOADAVG((struct loadavg *)oldp, *oldlenp);
			break;
		case VM_MAXSLP:	/* time to be blocked before being swappable (ps.exe) */
			*(int *)oldp = 0;
			break;
		case VM_UVMEXP:	/* statistics about the UVM memory management system (top.exe) */
			result = sysctl_VM_UVMEXP((struct uvmexp *)oldp, *oldlenp);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}

/****************************************************/

int 
sysctl_NET_RT(const int *name, void *buf, size_t *size)
{
	int result = 0;

	/* net/route.c */

	switch (name[4]){
		case NET_RT_DUMP:	/* 1 */
			result = route_NET_RT_DUMP(buf, size);
			break;
		case NET_RT_FLAGS:	/* 2 */
			result = route_NET_RT_FLAGS(name, buf, size);
			break;
		case NET_RT_IFLIST:	/* 3 */
			result = route_NET_RT_IFLIST(buf, size);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}
int 
sysctl_NET_INET(const int *name, void *buf, size_t *size)
{
	int result = 0;

	switch (name[2]){
		case IPPROTO_IP:	/* netinet/ip.c */
			result = ip_NET_INET_IP(name, buf, size);
			break;
		case IPPROTO_TCP:	/* netinet/tcp.c */
			result = tcp_NET_INET_TCP(name, buf, size);
			break;
		case IPPROTO_UDP:	/* netinet/udp.c */
			result = udp_NET_INET_UDP(name, buf, size);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}
int 
sysctl_NET_INET6(const int *name, void *buf, size_t *size)
{
	int result = 0;

	/* netinet/in.c */

	switch (name[2]){
		case IPPROTO_IPV6:	/* 41 */
			result = in_NET_INET6_IPV6(name, buf, size);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}
int 
sysctl_NET(const int *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = 0;

	switch (name[1]){
		case PF_ROUTE:
			result = sysctl_NET_RT(name, oldp, oldlenp);
			break;
		case PF_INET:
			result = sysctl_NET_INET(name, oldp, oldlenp);
			break;
		case PF_INET6:
			result = sysctl_NET_INET6(name, oldp, oldlenp);
			break;
		case PF_KEY:
			*oldlenp = 0;		/* no IPSEC (ifconfig.exe) */
			break;
		default:
			result = -ENOENT;
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
sysctl_VFS_GENERIC(const int *name, void *buf, size_t *size)
{
	int result = 0;

	switch (name[2]){
		case VFS_BCACHESTAT:		/* nbytes in VNODE cache (top.exe) */
			result = sysctl_VFS_BCACHESTAT((struct bcachestats *)buf, *size);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}
int 
sysctl_VFS(const int *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = 0;

	switch (name[1]){
		case VFS_GENERIC:
			result = sysctl_VFS_GENERIC(name, oldp, oldlenp);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}

/****************************************************/

int 
sysctl_MACHDEP_CPU_BIOS(const int *name, char **buf, size_t *size)
{
	int result = 0;

	switch (name[2]){
		case BIOS_DEV:
			*(dev_t *)buf = DEV_TYPE_PORT;
			break;
		case BIOS_DISKINFO:
			result = -ENXIO;
			break;
		case BIOS_CKSUMLEN:
		case BIOS_MAXID:
		default:
			result = -ENOENT;
	}
	return(result);
}
int 
sysctl_MACHDEP(const int *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = 0;

	switch (name[1]){
		case CPU_CONSDEV:
			*(dev_t *)oldp = DEV_TYPE_CONSOLE;
			break;
		case CPU_BIOS:
			result = sysctl_MACHDEP_CPU_BIOS(name, (char **)oldp, oldlenp);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}

/****************************************************/

int 
sys___sysctl(call_t call, const int *name, u_int namelen, void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (namelen < 2 || namelen >= CTL_MAXNAME){
		result = -EINVAL;
	}else switch(name[0]){
		case CTL_KERN:
			result = sysctl_KERN(name, oldp, oldlenp, newp, newlen);
			break;
		case CTL_HW:
			result = sysctl_HW(name, oldp, oldlenp, newp, newlen);
			break;
		case CTL_VM:
			result = sysctl_VM(name, oldp, oldlenp, newp, newlen);
			break;
		case CTL_NET:
			result = sysctl_NET(name, oldp, oldlenp, newp, newlen);
			break;
		case CTL_VFS:
			result = sysctl_VFS(name, oldp, oldlenp, newp, newlen);
			break;
		case CTL_MACHDEP:
			result = sysctl_MACHDEP(name, oldp, oldlenp, newp, newlen);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}
