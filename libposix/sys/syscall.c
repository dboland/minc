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

#include "sys/syscall.h"

#define MAXLINE		80
#define MAXDEPTH	16

/****************************************************/

PVOID 
env_win(char *const envp[])
{
	int size = 0;
	int len;
	char *entry = NULL;
	PVOID pvResult = win_malloc(WIN_PATH_MAX);
	char *p = pvResult;

	/* Keep Windows "Path" variable on top (login_passwd.exe).
	 * Keep Windows "SystemRoot" variable, needed for WSASocket().
	 */
	size = win_getenv("Path", pvResult, WIN_PATH_MAX) + 1;
	p += size;
	size += win_getenv("SystemRoot", p, MAX_PATH) + 1;
	while (entry = *envp++){
		if (!win_strncmp(entry, "SystemRoot=", 11)){
			continue;
		}else if (!win_strncmp(entry, "Path=", 5)){
			continue;
		}
		len = win_strlen(entry) + 1;
		pvResult = win_realloc(pvResult, size + len);
		p = pvResult + size;
		p = win_stpcpy(p, entry) + 1;
		size = p - (char *)pvResult;
	}
	*p = 0;
	return(pvResult);
}
LPSTR 
argv_win(WIN_TASK *Task, const char *command, char *const argv[])
{
	LPSTR pszResult = win_malloc(PATH_MAX);
	char *p;
	int size = 0;
	int len;
	int maxbuf = MAX_ARGBUF - (MAX_ARGBUF % MIN_BUFSIZE);
	char *arg = *argv++;		/* skip first argument (unresolved command) */

	p = win_stpcpy(pszResult, command);
	size = p - pszResult;
	while (arg = *argv++){
		len = win_strlen(arg) + 3;
		pszResult = win_realloc(pszResult, size + len);
		p = pszResult + size;
		p = stpquot(p, arg);
		size = p - pszResult;
		/* maximum for CreateProcess(), rounded to nearest block */
		if (size >= maxbuf){		/* xargs.exe */
			msvc_printf("+ warning: %s: Too many arguments\n", command);
			break;
		}
	}
	if (Task->TracePoints & KTRFAC_NAMEI){
		ktrace_NAMEI(Task, pszResult, size);
	}
	return(pszResult);
}
int
argv_posix(char *buf, int size, char *argv[])
{
	char c;
	int count = 0;

	*argv = buf;
	while (c = *buf){
		if (!size){
			break;
		}else if (c == '\r' || c == '\n'){
			break;
		}else if (strchr("#!", c)){
			buf++;
			*argv = buf;
		}else if (c != ' '){
			buf++;
		}else if (count >= MAXDEPTH){
			msvc_printf("+ warning: #!: Too many arguments\n");
			break;
		}else{
			*buf++ = 0;
			if (*argv[0]){		/* skip empty arguments */
				argv++;
				count++;
			}
			*argv = buf;
		}
		size--;
	}
	*buf = 0;
	count++;
	argv++;
	*argv = NULL;
	return(count);
}
BOOL 
shebang_win(WIN_VNODE *Node, WIN_NAMEIDATA *Path, const char *filename, LPSTR Result)
{
	BOOL bResult = FALSE;
	DWORD dwRead;
	WIN_NAMEIDATA wPath = {0};
	char *argv[MAXDEPTH + 1], **token = argv;
	char line[MAXLINE];

	if (Node->FileType == WIN_VDIR){
		SetLastError(ERROR_PATH_BUSY);
	}else if (Node->FSType != FS_TYPE_DISK){
		SetLastError(ERROR_ACCESS_DENIED);
	}else if (!vfs_read(Node, line, MAXLINE, &dwRead)){
		bResult = FALSE;
	}else if (line[0] == 'M' && line[1] == 'Z'){	/* hello Mark Zbikowski */
		win_wcstombs(Result, Path->Resolved, WIN_PATH_MAX);
		bResult = TRUE;
	}else if (line[0] != '#' || line[1] != '!'){
		SetLastError(ERROR_BAD_EXE_FORMAT);
	}else if (argv_posix(line, MAXLINE, argv)){
		Result = win_wcstombp(Result, path_win(&wPath, *token++, 0)->Resolved, WIN_PATH_MAX);
		while (*token){
			Result = win_stpcpy(win_stpcpy(Result, " "), *token++);
		}
		Result = win_stpcpy(win_stpcpy(Result, " "), filename);
		bResult = TRUE;
	}
	vfs_close(Node);
	return(bResult);
}

/****************************************************/

uid_t 
__getuid(WIN_TASK *Task)
{
	uid_t uid = Task->RealUid;
	SID8 sidUser;

	if (!uid){
		uid = rid_posix(win_getuid(&sidUser));
		Task->RealUid = uid;
	}
	if (uid == WIN_ROOT_UID){
		uid = 0;
	}
	return(uid);
}
uid_t 
sys_getuid(call_t call)
{
	return(__getuid(call.Task));
}
uid_t 
__geteuid(WIN_TASK *Task)
{
	uid_t uid = rid_posix(&Task->UserSid);

	if (uid == WIN_ROOT_UID){
		uid = 0;
	}
	return(uid);
}
uid_t 
sys_geteuid(call_t call)
{
	return(__geteuid(call.Task));
}
int 
sys_getresuid(call_t call, uid_t *ruid, uid_t *euid, uid_t *suid)
{
	WIN_TASK *pwTask = call.Task;

	*euid = __geteuid(pwTask);
	*ruid = __getuid(pwTask);
	*suid = pwTask->SavedUid;
	return(0);
}
gid_t 
__getgid(WIN_TASK *Task)
{
	gid_t gid = Task->RealGid;
	SID8 sidGroup;

	if (!gid){
		gid = rid_posix(win_getgid(&sidGroup));
		Task->RealGid = gid;
	}
	if (gid == WIN_ROOT_GID){
		gid = 0;
	}
	return(gid);
}
gid_t 
sys_getgid(call_t call)
{
	return(__getgid(call.Task));
}
gid_t 
__getegid(WIN_TASK *Task)
{
	gid_t gid = rid_posix(&Task->GroupSid);

	if (gid == WIN_ROOT_GID){
		gid = 0;
	}
	return(gid);
}
gid_t 
sys_getegid(call_t call)
{
	return(__getegid(call.Task));
}
int 
sys_getresgid(call_t call, gid_t *rgid, gid_t *egid, gid_t *sgid)
{
	WIN_TASK *pwTask = call.Task;

	*egid = __getegid(pwTask);
	*rgid = __getgid(pwTask);
	*sgid = pwTask->SavedGid;
	return(0);
}

/*******************************************************/

int 
__seteuid(WIN_TASK *Task, uid_t uid)
{
	int result = 0;
	SID8 sidUser;

	if (!uid){
		uid = WIN_ROOT_UID;
	}
	if (uid == rid_posix(&Task->UserSid)){		/* PRIV_START check (ssh.exe) */
		return(0);
	}else if (__getuid(Task) && __geteuid(Task)){
		result = -EPERM;
	}else if (!vfs_seteuid(Task, rid_win(&sidUser, uid))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_seteuid(call_t call, uid_t uid)
{
	return(__seteuid(call.Task, uid));
}
int 
__setuid(WIN_TASK *Task, uid_t uid)
{
	int result = 0;

	if (!uid){
		uid = WIN_ROOT_UID;
	}
	if (uid == Task->RealUid){
		return(0);
	}else if (!__seteuid(Task, uid)){
		Task->RealUid = rid_posix(&Task->UserSid);
	}else{
		result = -EPERM;
	}
	return(result);
}
int 
sys_setuid(call_t call, uid_t uid)
{
	return(__setuid(call.Task, uid));
}
int 
__setreuid(WIN_TASK *Task, uid_t ruid, uid_t euid)
{
	int result = 0;

	if (!ruid){
		ruid = WIN_ROOT_UID;
	}
	if (ruid == __getuid(Task)){
		result = __seteuid(Task, euid);
	}else{
		result = __setuid(Task, ruid);
	}
	return(result);
}
int 
sys_setreuid(call_t call, uid_t ruid, uid_t euid)
{
	return(__setreuid(call.Task, ruid, euid));
}
int 
sys_setresuid(call_t call, uid_t ruid, uid_t euid, uid_t suid)
{
	int result = 0;

	if (!suid){
		suid = WIN_ROOT_UID;
	}
	if (!__setreuid(call.Task, ruid, euid)){
		call.Task->SavedUid = suid;
	}else{
		result = -EPERM;
	}
	return(result);
}

/****************************************************/

int 
__setegid(WIN_TASK *Task, gid_t gid)
{
	int result = 0;
	SID8 sidGroup;

	if (!gid){
		gid = WIN_ROOT_GID;
	}
	if (gid == rid_posix(&Task->GroupSid)){		/* PRIV_START check (ssh.exe) */
		return(0);
	}else if (__getuid(Task) && __geteuid(Task)){
		result = -EPERM;
	}else if (!vfs_setegid(Task, rid_win(&sidGroup, gid))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_setegid(call_t call, gid_t gid)
{
	return(__setegid(call.Task, gid));
}
int 
__setgid(WIN_TASK *Task, gid_t gid)
{
	int result = 0;

	if (!gid){
		gid = WIN_ROOT_GID;
	}
	if (gid == Task->RealGid){
		result = 0;
	}else if (__setegid(Task, gid)){
		result -= errno_posix(GetLastError());
	}else{
		Task->RealGid = rid_posix(&Task->GroupSid);
	}
	return(result);
}
int 
sys_setgid(call_t call, gid_t gid)
{
	return(__setgid(call.Task, gid));
}
int 
__setregid(WIN_TASK *Task, gid_t rgid, gid_t egid)
{
	int result = 0;

	if (rgid == Task->RealGid){
		result = __setegid(Task, egid);
	}else{
		result = __setgid(Task, rgid);
	}
	return(result);
}
int 
sys_setregid(call_t call, gid_t rgid, gid_t egid)
{
	return(__setregid(call.Task, rgid, egid));
}
int 
sys_setresgid(call_t call, gid_t rgid, gid_t egid, gid_t sgid)
{
	if (!sgid){
		sgid = WIN_ROOT_GID;
	}
	if (!__setregid(call.Task, rgid, egid)){
		call.Task->SavedGid = sgid;
	}else{
		return(-EPERM);
	}
	return(0);
}

/****************************************************/

int 
sys_getlogin(call_t call, char *name, size_t namelen)
{
	int result = 0;
	SID8 sid;

	if (!vfs_getlogin(call.Task, name, namelen)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_setlogin(call_t call, const char *name)
{
	int result = 0;

	if (!vfs_setlogin(call.Task, name)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}

/****************************************************/

pid_t 
sys_getpid(call_t call)
{
	return(call.Task->TaskId);
}
pid_t 
sys_getppid(call_t call)
{
	return(call.Task->ParentId);
}
pid_t 
sys_getsid(call_t call, pid_t pid)
{
	pid_t result = 0;
	WIN_TASK *pwTask = call.Task;

	if (pid < 0 || pid >= CHILD_MAX){
		result = -EINVAL;
	}else if (!pid){
		result = pwTask->SessionId;
	}else{
		result = __Tasks[pid].SessionId;
	}
	return(result);
}
pid_t 
sys_setsid(call_t call)
{
	pid_t result = 0;
	WIN_TASK *pwTask = call.Task;

	if (!vfs_setsid(pwTask)){
		result -= errno_posix(GetLastError());
	}else{
		result = pwTask->SessionId;
	}
	return(result);
}
int 
sys_chown(call_t call, const char *path, uid_t owner, gid_t group)
{
	int result = 0;
	SID8 sidUser;
	SID8 sidGroup;
	WIN_NAMEIDATA wpePath = {0};

	if (!owner){
		owner = WIN_ROOT_UID;
	}
	if (!group){
		group = WIN_ROOT_GID;
	}
	if (!vfs_chown(path_win(&wpePath, path, 0), rid_win(&sidUser, owner), rid_win(&sidGroup, group))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_fchown(call_t call, int fd, uid_t owner, gid_t group)
{
	int result = 0;
	SID8 sidUser;
	SID8 sidGroup;
	WIN_TASK *pwTask = call.Task;

	if (!owner){
		owner = WIN_ROOT_UID;
	}
	if (!group){
		group = WIN_ROOT_GID;
	}
	if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_fchown(&pwTask->Node[fd], rid_win(&sidUser, owner), rid_win(&sidGroup, group))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_lchown(call_t call, const char *path, uid_t owner, gid_t group)
{
	int result = 0;
	SID8 sidUser;
	SID8 sidGroup;
	WIN_NAMEIDATA wpePath = {0};

	if (!owner){
		owner = WIN_ROOT_UID;
	}
	if (!group){
		group = WIN_ROOT_GID;
	}
	if (!vfs_chown(path_win(&wpePath, path, O_NOFOLLOW), rid_win(&sidUser, owner), rid_win(&sidGroup, group))){
		result -= errno_posix(GetLastError());
	}else{
		result = 0;
	}
	return(0);
}
int 
sys_fchownat(call_t call, int dirfd, const char *path, uid_t owner, gid_t group, int flags)
{
	int result = 0;
	SID8 sidUser;
	SID8 sidGroup;
	WIN_NAMEIDATA wPath = {0};

	if (!owner){
		owner = WIN_ROOT_UID;
	}
	if (!group){
		group = WIN_ROOT_GID;
	}
	if (!path || !path[0]){
		result = -EINVAL;
	}else if (!vfs_chown(pathat_win(&wPath, dirfd, path, flags), rid_win(&sidUser, owner), rid_win(&sidGroup, group))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_pipe(call_t call, int pipefd[2])
{
	int result = 0;
	WIN_VNODE vnResult[2] = {0};
	WIN_TASK *pwTask = call.Task;

	if (!pipefd){
		result = -EFAULT;
	}else if (!vfs_pipe(vnResult)){
		result -= errno_posix(GetLastError());
	}else{
		pipefd[0] = fd_posix(pwTask, &vnResult[0], 0);
		pipefd[1] = fd_posix(pwTask, &vnResult[1], 0);
		if (pwTask->TracePoints & KTRFAC_STRUCT){
			ktrace_STRUCT(pwTask, "fdvec", 5, pipefd, (sizeof(int) * 2));
		}
	}
	return(result);
}
int 
sys_execve(call_t call, const char *path, char *const argv[], char *const envp[])
{
	int result = 0;
	WIN_NAMEIDATA wPath = {0};
	WIN_TASK *pwTask = call.Task;
	WIN_MODE wMode;
	WIN_FLAGS wFlags = {GENERIC_READ + GENERIC_EXECUTE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0};
	WIN_VNODE vNode = {0};
	CHAR szCommand[PATH_MAX] = "";

	if (!path || !envp){
		result = -EFAULT;
	}else if (!vfs_open(path_win(&wPath, path, 0), &wFlags, mode_win(&wMode, 0666), &vNode)){
		result -= errno_posix(GetLastError());
	}else if (!shebang_win(&vNode, &wPath, path, szCommand)){
		result -= errno_posix(GetLastError());
	}else if (!vfs_execve(pwTask, argv_win(pwTask, szCommand, argv), env_win(envp))){
		result -= errno_posix(GetLastError());
	}else{
		win_exit(0);
	}
	return(result);
}
__dead void 
__exit(WIN_TASK *Task, int status)
{
	Task->Status = (status * 0x100);
	Task->Flags |= WIN_PS_EXITING;
	if (Task->Flags & WIN_PS_CONTROLT){
		pdo_revoke(DEVICE(__CTTY->DeviceId));
	}
	win_exit(status);
}
__dead void 
sys_exit(call_t call, int status)
{
	__exit(call.Task, status);
}
__dead void 
sys___threxit(call_t call, pid_t *pid)
{
	__exit(call.Task, 0);
}
/* void *
sys___get_tcb(call_t call)
{
	return(call.Task);
} */
int 
sys_interrupt(call_t call)
{
	return(-EINTR);
}
int 
sys_nosys(call_t call)
{
	return(-ENOSYS);
}

/****************************************************/

const void *
syscall_enter(call_t call)
{
	int code = call.Code;
	struct sysent *ent = &sysent[code];
	void *result = ent->sy_call;
	WIN_TASK *pwTask = &__Tasks[CURRENT];

	if (pwTask->TracePoints & KTRFAC_SYSCALL){
		ktrace_SYSCALL(pwTask, code, ent->sy_argsize, &call.Base + 1);
	}
	if (pwTask->Timer){		/* ftp.exe */
		WaitForSingleObjectEx(__Interrupt, 0, TRUE);
	}
	if (proc_poll()){
		result = sys_interrupt;
	}
	pwTask->Code = code;
	call.Task = pwTask;
	return(result);
}
void 
syscall_leave(call_t call)
{
	int result = call.c_ax;
	int code = call.Code;
	WIN_TASK *pwTask = call.Task;

	if (result < 0){
		pwTask->Error = -result;
	}
	/* Note: WriteFile() will touch the %edx register.
	 */
	if (pwTask->TracePoints & KTRFAC_SYSRET){
		ktrace_SYSRET(pwTask, code, result);
	}
}

