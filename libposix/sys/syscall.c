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
	PVOID pvResult = win_malloc(PATH_MAX * 2);
	char *p = pvResult;
	int count = 1;

	/* Keep Windows "Path" variable on top (login_passwd.exe) */
	size = win_getenv("Path", pvResult, PATH_MAX * 2) + 1;
	while (entry = *envp++){
		if (win_strncmp(entry, "Path=", 5)){
			len = win_strlen(entry) + 1;
			pvResult = win_realloc(pvResult, size + len);
			p = pvResult + size;
			p = win_stpcpy(p, entry) + 1;
			size = p - (char *)pvResult;
			count++;
		}
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
	WIN_NAMEIDATA wPath;
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
	disk_close(Node);
	return(bResult);
}

/****************************************************/

uid_t 
__getuid(WIN_TASK *Task)
{
	SID8 sidUser;
	uid_t uid = Task->RealUid;

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
__geteuid(WIN_TASK *Task)
{
	SID8 sidUser;
	uid_t uid = rid_posix(&Task->UserSid);

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
sys_geteuid(call_t call)
{
	return(__geteuid(call.Task));
}
int 
sys_getresuid(call_t call, uid_t *ruid, uid_t *euid, uid_t *suid)
{
	WIN_TASK *pwTask = call.Task;

	*euid = __geteuid(pwTask);
	*ruid = pwTask->RealUid;
	*suid = pwTask->SavedUid;
	return(0);
}
gid_t 
__getgid(WIN_TASK *Task)
{
	SID8 sidGroup;
	gid_t gid = Task->RealGid;

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
__getegid(WIN_TASK *Task)
{
	SID8 sidGroup;
	gid_t gid = rid_posix(&Task->GroupSid);

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
sys_getegid(call_t call)
{
	return(__getegid(call.Task));
}
int 
sys_getresgid(call_t call, gid_t *rgid, gid_t *egid, gid_t *sgid)
{
	WIN_TASK *pwTask = call.Task;

	*egid = __getegid(pwTask);
	*rgid = pwTask->RealGid;
	*sgid = pwTask->SavedGid;
	return(0);
}

/*******************************************************/

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
	WIN_NAMEIDATA wpePath;

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
	WIN_NAMEIDATA wpePath;

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
sys_fchownat(call_t call, int dirfd, const char *pathname, uid_t owner, gid_t group, int flags)
{
	int result = 0;
	SID8 sidUser;
	SID8 sidGroup;
	WIN_NAMEIDATA wpePath;

	if (!owner){
		owner = WIN_ROOT_UID;
	}
	if (!group){
		group = WIN_ROOT_GID;
	}
	if (!pathname || !pathname[0]){
		result = -EINVAL;
	}else if (!vfs_chown(pathat_win(&wpePath, dirfd, pathname, flags), rid_win(&sidUser, owner), rid_win(&sidGroup, group))){
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
	WIN_NAMEIDATA wPath;
	WIN_TASK *pwTask = call.Task;
	WIN_MODE wMode;
	WIN_FLAGS wFlags = {GENERIC_READ + GENERIC_EXECUTE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0};
	WIN_VNODE vNode = {0};
	CHAR szCommand[PATH_MAX] = "";

	if (!path || !envp){
		result = -EINVAL;
	}else if (!disk_open(path_win(&wPath, path, 0), &wFlags, mode_win(&wMode, 0666), &vNode)){
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
		tty_close(__CTTY);
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

	pwTask->Code = code;
	if (pwTask->TracePoints & KTRFAC_SYSCALL){
		ktrace_SYSCALL(pwTask, code, ent->sy_argsize, &call.Base + 1);
	}
	if (proc_poll()){
		result = sys_interrupt;
	}
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
		ktrace_SYSRET(pwTask, call.Code, result);
	}
}

