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

#include <unistd.h>

/*******************************************************/

SID8 *
groups_win(DWORD *Count)
{
	DWORD dwCount = *Count;
	SID8 *sidResult = win_malloc(sizeof(SID8) * (dwCount + 4));

	/* Administrators */
	/* bypass traverse checking (SeChangeNotifyPrivilege) */
	sidResult[dwCount++] = SidEveryone;
	/* WinStation/Desktop access */
	sidResult[dwCount++] = SidRestricted;
	/* Shared memory access */
	sidResult[dwCount++] = SidAuthenticated;
	*Count = dwCount;
	return(sidResult);
}

/*******************************************************/

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

long 
sys_pathconf(call_t call, const char *path, int name)
{
	long result = 0;

	switch (name){
		case _PC_MAX_CANON:
			result = MAX_CANON;
			break;
		case _PC_MAX_INPUT:
			result = WIN_MAX_INPUT;
			break;
//		case _PC_VDISABLE:
//			break;
		case _PC_NAME_MAX:
			result = WIN_NAME_MAX;
			break;
		case _PC_PATH_MAX:
			result = WIN_PATH_MAX;
			break;
		case _PC_PIPE_BUF:
			result = WIN_PIPE_BUF;
			break;
		default:
			__PRINTF("sys_pathconf(%s) name(%d)\n", path, name)
			result = -EOPNOTSUPP;
	}
	return(result);
}
long 
sys_fpathconf(call_t call, int fd, int name)
{
	return(sys_pathconf(call, "", name));
}
int 
sys_getdtablecount(call_t call)
{
	int result = 0;
	WIN_VNODE *pvNode = call.Task->Node;
	int fd = 0;

	while (fd < OPEN_MAX){
		if (pvNode->Access){
			result++;
		}
		fd++;
		pvNode++;
	}
	return(result);
}

/****************************************************/

int 
sys_getgroups(call_t call, int size, gid_t list[])
{
	int result = 0;
	SID8 *grList = NULL;
	DWORD dwCount = 0;
	int index = 0;

	if (size < 0){	/* GNU conftest.exe */
		result = -EINVAL;
	}else if (!win_getgroups(&grList, &dwCount)){
		result -= errno_posix(GetLastError());
	}else if (size >= dwCount){
		while (index < dwCount){
			list[index] = rid_posix(&grList[index]);
			if (list[index] == WIN_ROOT_GID){
				list[index] = 0;
			}
			index++;
		}
		result = dwCount;
	}
	win_free(grList);
	return(result);
}
int 
sys_setgroups(call_t call, int size, const gid_t *list)
{
	int result = 0;
	SID8 sid;
	SID8 *grList;
	DWORD dwIndex = 0;
	DWORD dwCount = size;

	if (size < 0 || size >= NGROUPS_MAX){
		result = -EINVAL;
	}else{
		grList = groups_win(&dwCount);
		while (size--){
			if (!list[size]){
				grList[dwIndex] = *rid_win(&sid, WIN_ROOT_GID);
			}else{
				grList[dwIndex] = *rid_win(&sid, list[size]);
			}
			dwIndex++;
		}
		if (!win_setgroups(grList, dwCount)){
			result -= errno_posix(GetLastError());
		}
		win_free(grList);
	}
	return(result);
}

/****************************************************/

int 
sys_rmdir(call_t call, const char *pathname)
{
	int result = 0;
	WIN_NAMEIDATA wPath;

	if (!vfs_rmdir(path_win(&wPath, pathname, O_NOFOLLOW))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
__linkat(WIN_NAMEIDATA *Path, int fd2, const char *name2, int flag)
{
	int result = 0;
	WIN_NAMEIDATA wpNew;

	if (!vfs_link(Path, pathat_win(&wpNew, fd2, name2, flag))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_link(call_t call, const char *name1, const char *name2)
{
	WIN_NAMEIDATA wpOld;

	return(__linkat(path_win(&wpOld, name1, 0), AT_FDCWD, name2, AT_SYMLINK_FOLLOW));
}
int 
sys_linkat(call_t call, int fd1, const char *name1, int fd2, const char *name2, int flag)
{
	WIN_NAMEIDATA wpOld;

	return(__linkat(pathat_win(&wpOld, fd1, name1, flag), fd2, name2, flag));
}
int 
__unlinkat(WIN_TASK *Task, int fd, const char *path, int flag)
{
	int result = 0;
	WIN_NAMEIDATA wPath;
	BOOL bResult = FALSE;

	if (!path){
		result = -EFAULT;
	}else if (!*path){
		result = -ENOENT;
	}else if (flag & AT_REMOVEDIR){
		bResult = vfs_rmdir(pathat_win(&wPath, fd, path, AT_SYMLINK_NOFOLLOW));
	}else{
		bResult = vfs_unlink(pathat_win(&wPath, fd, path, AT_SYMLINK_NOFOLLOW));
	}
	if (!bResult){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_unlinkat(call_t call, int fd, const char *path, int flag)
{
	return(__unlinkat(call.Task, fd, path, flag));
}
int 
sys_unlink(call_t call, const char *path)
{
	return(__unlinkat(call.Task, AT_FDCWD, path, 0));
}
void 
sys_sync(call_t call)
{
	vfs_sync();
}
int 
sys_fsync(call_t call, int fd)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_fsync(&pwTask->Node[fd])){
		result -= errno_posix(GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
int 
sys_setdtablesize(call_t call, int newsize)
{
	msvc_printf("  setdtablesize(%d)\n", newsize);
	return(0);
}
int 
sys___getcwd(call_t call, char *buf, size_t size)
{
	WIN_TASK *pwTask = call.Task;

	pathn_posix(buf, pwTask->Path.Name, size);
	if (pwTask->Flags & PS_TRACED){
		ktrace_NAMEI(pwTask, buf, win_strlen(buf));
	}
	return(0);
}
int 
sys_fchdir(call_t call, int fildes)
{
	int result = 0;
	WIN_NAMEIDATA wPath;
	WIN_TASK *pwTask = call.Task;

	if (!vfs_chdir(pwTask, pathat_win(&wPath, fildes, "", 0))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_chdir(call_t call, const char *path)
{
	int result = 0;
	WIN_NAMEIDATA wPath;
	WIN_TASK *pwTask = call.Task;

	if (!vfs_chdir(pwTask, path_win(&wPath, path, 0))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_chroot(call_t call, const char *path)
{
	int result = 0;
	WIN_NAMEIDATA wPath;

	if (!vfs_chroot(path_win(&wPath, path, 0))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
ssize_t 
__readlinkat(WIN_TASK *Task, int dirfd, const char *pathname, char *buf, size_t bufsiz)
{
	ssize_t result = 0;
	WIN_NAMEIDATA wpePath;

	if (!disk_readlink(pathat_win(&wpePath, dirfd, pathname, AT_SYMLINK_NOFOLLOW), FALSE)){
		result -= errno_posix(GetLastError());
	}else{
		result = pathnp_posix(buf, wpePath.Resolved, bufsiz, TRUE) - buf;
	}
	return(result);
}
ssize_t 
sys_readlinkat(call_t call, int dirfd, const char *pathname, char *buf, size_t bufsiz)
{
	return(__readlinkat(call.Task, dirfd, pathname, buf, bufsiz));
}
int 
sys_readlink(call_t call, const char *path, char *buf, size_t bufsiz)
{
	return(__readlinkat(call.Task, AT_FDCWD, path, buf, bufsiz));
}
int 
__symlinkat(WIN_NAMEIDATA *Target, int fd, const char *path)
{
	int result = 0;
	WIN_NAMEIDATA wnPath;
	WIN_VATTR wStat = {0};
	WIN_MODE wMode;

	if (Target->Attribs != -1){
		vfs_stat(Target, &wStat);
	}
	if (!disk_symlink(Target, &wStat, mode_win(&wMode, 00755), pathat_win(&wnPath, fd, path, AT_SYMLINK_NOFOLLOW))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_symlinkat(call_t call, const char *name1, int fd, const char *name2)
{
	WIN_NAMEIDATA wnTarget;

	return(__symlinkat(pathat_win(&wnTarget, 0, name1, AT_REQUIREDRIVE), fd, name2));
}
int 
sys_symlink(call_t call, const char *name1, const char *name2)
{
	WIN_NAMEIDATA wnTarget;

	return(__symlinkat(pathat_win(&wnTarget, 0, name1, AT_REQUIREDRIVE), AT_FDCWD, name2));
}
int 
__faccessat(int dirfd, const char *path, int mode, int flags)
{
	int result = 0;
	WIN_NAMEIDATA wPath;
	ACCESS_MASK amDesired = 0;

	if (mode & R_OK){
		amDesired |= WIN_S_IREAD;
	}
	if (mode & W_OK){
		amDesired |= WIN_S_IWRITE;
	}
	if (mode & X_OK){
		amDesired |= WIN_S_IEXEC;
	}
	if (!vfs_access(pathat_win(&wPath, dirfd, path, flags), amDesired)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_faccessat(call_t call, int dirfd, const char *pathname, int mode, int flags)
{
	return(__faccessat(dirfd, pathname, mode, flags));
}
int 
sys_access(call_t call, const char *pathname, int mode)
{
	return(__faccessat(AT_FDCWD, pathname, mode, AT_SYMLINK_FOLLOW));
}
int 
__renameat(WIN_NAMEIDATA *Path, int newdirfd, const char *newpath)
{
	int result = 0;
	WIN_NAMEIDATA wpNew;

	if (Path->Attribs == -1){
		result -= errno_posix(GetLastError());
	}else if (!vfs_rename(Path, pathat_win(&wpNew, newdirfd, newpath, AT_SYMLINK_NOFOLLOW))){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_renameat(call_t call, int dirfd, const char *path, int newdirfd, const char *newpath)
{
	WIN_NAMEIDATA wpOld;

	return(__renameat(pathat_win(&wpOld, dirfd, path, AT_SYMLINK_NOFOLLOW), newdirfd, newpath));
}
int 
sys_rename(call_t call, const char *path, const char *newpath)
{
	WIN_NAMEIDATA wpOld;

	return(__renameat(path_win(&wpOld, path, O_NOFOLLOW), AT_FDCWD, newpath));
}
int 
sys_ftruncate(call_t call, int fd, off_t length)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;
	LARGE_INTEGER liOffset = {length & 0xFFFFFFFF, length >> 32};

	if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_ftruncate(&pwTask->Node[fd], &liOffset)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_truncate(call_t call, const char *path, off_t length)
{
	int result = 0;
	WIN_NAMEIDATA wPath;
	LARGE_INTEGER liOffset = {length & 0xFFFFFFFF, length >> 32};

	if (!vfs_truncate(path_win(&wPath, path, 0), &liOffset)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}

/****************************************************/

int 
sys_dup(call_t call, int oldfd)
{
	int result = 0;
	WIN_VNODE vnResult = {0};
	WIN_TASK *pwTask = call.Task;

	if (oldfd < 0 || oldfd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_dup(&pwTask->Node[oldfd], &vnResult)){
		result -= errno_posix(GetLastError());
	}else{
		result = fd_posix(pwTask, &vnResult, 0);
	}
	return(result);
}
int 
sys_dup2(call_t call, int oldfd, int newfd)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (oldfd < 0 || oldfd >= OPEN_MAX){
		result = -EBADF;
	}else if (newfd < 0 || newfd >= OPEN_MAX){
		result = -EINVAL;
	}else if (!vfs_dup2(&pwTask->Node[oldfd], &pwTask->Node[newfd])){
		result -= errno_posix(GetLastError());
	}else{
		result = newfd;
	}
	return(result);
}
off_t 
sys_lseek(call_t call, int fd, off_t offset, int whence)
{
	off_t result = 0LL;
	DWORD dwMethod = FILE_BEGIN;
	LARGE_INTEGER liResult;
	WIN_TASK *pwTask = call.Task;
	LARGE_INTEGER liOffset = {offset & 0xFFFFFFFF, offset >> 32};

	if (whence == SEEK_CUR){
		dwMethod = FILE_CURRENT;
	}else if (whence == SEEK_END){
		dwMethod = FILE_END;
	}
	if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_lseek(&pwTask->Node[fd], &liOffset, dwMethod, &liResult)){
		result -= errno_posix(GetLastError());
	}else{
		result = liResult.QuadPart;
	}
	return(result);
}
int 
sys_close(call_t call, int fd)
{
	int result = 0;
	WIN_TASK *pwTask = call.Task;

	if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_close(&pwTask->Node[fd])){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
__closefrom(WIN_VNODE Nodes[], int offset)
{
	WIN_VNODE *pvNode = &Nodes[offset];

	while (offset < OPEN_MAX){
		if (pvNode->Access){
			vfs_close(pvNode);
		}
		offset++;
		pvNode++;
	}
	return(0);
}
int 
sys_closefrom(call_t call, int fd)
{
	int result = 0;

	if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else{
		result = __closefrom(call.Task->Node, fd);
	}
	return(result);
}
ssize_t 
sys_read(call_t call, int fd, void *buf, size_t count)
{
	ssize_t result = 0;
	DWORD dwResult = -1;		/* WinNT EOF on ReadFile() */
	DWORD dwCount = count;
	WIN_TASK *pwTask = call.Task;
	CHAR szMessage[MAX_MESSAGE];

	if (pwTask->TracePoints & KTRFAC_USER){
		ktrace_USER(pwTask, "VNODE", szMessage, vfs_ktrace(&pwTask->Node[fd], szMessage));
	}
	if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_read(&pwTask->Node[fd], buf, dwCount, &dwResult)){
		result -= errno_posix(GetLastError());
	}else if (dwResult == -1){
		pwTask->Error = errno_posix(GetLastError());
	}else{
		result = dwResult;
		if (pwTask->TracePoints & KTRFAC_GENIO){
			ktrace_GENIO(pwTask, fd, UIO_READ, buf, dwResult);
		}
	}
	return(result);
}
ssize_t 
sys_write(call_t call, int fd, const void *buf, size_t nbytes)
{
	ssize_t result = 0;
	DWORD dwResult = 0;		/* WinNT EOF on WriteFile() */
	WIN_TASK *pwTask = call.Task;
	CHAR szMessage[MAX_MESSAGE];

	if (pwTask->TracePoints & KTRFAC_USER){
		ktrace_USER(pwTask, "VNODE", szMessage, vfs_ktrace(&pwTask->Node[fd], szMessage));
	}
	if (pwTask->TracePoints & KTRFAC_GENIO){
		ktrace_GENIO(pwTask, fd, UIO_WRITE, buf, nbytes);
	}
	if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_write(&pwTask->Node[fd], buf, nbytes, &dwResult)){
		result -= errno_posix(GetLastError());
	}else if (!dwResult){
		pwTask->Error = errno_posix(GetLastError());
	}else{
		result = dwResult;
	}
	return(result);
}
ssize_t 
sys_pread(call_t call, int fd, void *buf, size_t nbytes, off_t offset)
{
	ssize_t result = 0;
	DWORD dwResult = -1;
	WIN_TASK *pwTask = call.Task;

	if (offset < 0){
		result = -EINVAL;
	}else if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_pread(&pwTask->Node[fd], buf, nbytes, offset, &dwResult)){
		result -= errno_posix(GetLastError());
	}else{
		result = dwResult;
	}
	return(result);
}
ssize_t 
sys_pwrite(call_t call, int fd, const void *buf, size_t nbytes, off_t offset)
{
	ssize_t result = 0;
	DWORD dwResult = -1;
	WIN_TASK *pwTask = call.Task;

	if (offset < 0){
		result = -EINVAL;
	}else if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else if (!vfs_pwrite(&pwTask->Node[fd], buf, nbytes, offset, &dwResult)){
		result -= errno_posix(GetLastError());
	}else{
		result = dwResult;
	}
	return(result);
}

/****************************************************/

int 
sys_getthrid(call_t call)
{
	return(call.Task->TaskId);	/* libc.so raise() (git.exe) */
}
pid_t 
__getpgid(WIN_TASK *Task, pid_t pid)
{
	pid_t result = 0;

	if (pid < 0 || pid >= CHILD_MAX){
		result = -EINVAL;
	}else if (!pid){
		result = Task->GroupId;
	}else{
		result = __Tasks[pid].GroupId;
	}
	return(result);
}
pid_t 
sys_getpgid(call_t call, pid_t pid)
{
	return(__getpgid(call.Task, pid));
}
pid_t 
sys_getpgrp(call_t call)
{
	return(__getpgid(call.Task, 0));
}
int 
sys_setpgid(call_t call, pid_t pid, pid_t pgid)
{
	int result = 0;

	if (pid < 0 || pid >= CHILD_MAX){
		result = -EINVAL;
	}else{
		if (!pid){
			pid = call.Task->TaskId;
		}
		if (!pgid){
			pgid = __Tasks[pid].TaskId;
		}
		__Tasks[pid].GroupId = pgid;
	}
	return(result);
}
int 
sys_issetugid(call_t call)
{
	return(call.Task->IsSetUGid);
}
int 
sys_revoke(call_t call, const char *path)
{
	int result = 0;
	WIN_NAMEIDATA wPath;
	WIN_FLAGS wFlags = {GENERIC_READ + GENERIC_WRITE, 
		FILE_SHARE_READ, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 0};
	WIN_MODE wMode = {0};
	WIN_VNODE vNode = {0};

	if (!vfs_open(path_win(&wPath, path, 0), &wFlags, mode_win(&wMode, 0700), &vNode)){
		result -= errno_posix(GetLastError());
	}else if (!vfs_revoke(&vNode)){
		result -= errno_posix(GetLastError());
	}else if (!vfs_close(&vNode)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
sys_acct(call_t call, const char *filename)
{
	msvc_printf("acct(%s)\n", filename);
	return(0);
}
int 
sys_profil(call_t call, char *samples, size_t size, u_long offset, u_int scale)
{
	int result = 0;

	if (scale && !samples){
		result = -EFAULT;
	}
	return(result);
}
int 
sys_obreak(call_t call, char *nsize)
{
	msvc_printf("obreak(%d)\n", *nsize);
	return(0);
}
pid_t 
__tfork_thread(const struct __tfork *params, size_t psize, void (*startfunc)(void *), void *startarg)
{
	pid_t result = 0;
	DWORD dwResult = 0;

	if (!win___tfork_thread(NULL, psize, (PVOID)startfunc, startarg, &dwResult)){
		result -= errno_posix(GetLastError());
	}else{
		result = dwResult;
	}
	return(result);	
}
