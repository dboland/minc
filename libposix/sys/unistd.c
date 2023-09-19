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
	if (uid == __RootUid){
		uid = 0;
	}
	return(uid);
}
uid_t 
__geteuid(WIN_TASK *Task)
{
	SID8 sidUser;
	uid_t uid = rid_posix(&Task->UserSid);

	if (uid == __RootUid){
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
	if (gid == __RootGid){
		gid = 0;
	}
	return(gid);
}
gid_t 
__getegid(WIN_TASK *Task)
{
	SID8 sidGroup;
	gid_t gid = rid_posix(&Task->GroupSid);

	if (gid == __RootGid){
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
	long result = -1;

	switch (name){
		case _PC_MAX_CANON:
			result = MAX_CANON;
			break;
		case _PC_MAX_INPUT:
			result = MAX_INPUT;
			break;
//		case _PC_VDISABLE:
//			result = 0;
//			break;
		case _PC_NAME_MAX:
			result = NAME_MAX;
			break;
		case _PC_PATH_MAX:
			result = PATH_MAX;
			break;
		case _PC_PIPE_BUF:
			result = PIPE_BUF;
			break;
		default:
			__PRINTF("pathconf(%s) name(%d)\n", path, name)
			__errno_posix(call.Task, ERROR_NOT_SUPPORTED);
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
	SID8 *grList = NULL;
	DWORD dwIndex = 0;
	DWORD dwCount = -1;

	if (size < 0){	/* GNU conftest.exe */
		__errno_posix(call.Task, ERROR_BAD_ARGUMENTS);
	}else if (!win_getgroups(&grList, &dwCount)){
		__errno_posix(call.Task, GetLastError());
	}else if (size >= dwCount){
		while (dwIndex < dwCount){
			list[dwIndex] = rid_posix(&grList[dwIndex]);
			if (list[dwIndex] == WIN_ROOT_GID){
				list[dwIndex] = 0;
			}
			dwIndex++;
		}
	}
	win_free(grList);
	return(dwCount);
}
int 
sys_setgroups(call_t call, int size, const gid_t *list)
{
	int result = -1;
	SID8 sid;
	SID8 *grList;
	DWORD dwIndex = 0;
	DWORD dwCount = size;

	if (size < 0 || size >= NGROUPS_MAX){
		__errno_posix(call.Task, ERROR_BAD_ARGUMENTS);
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
			__errno_posix(call.Task, GetLastError());
		}else{
			result = 0;
		}
		win_free(grList);
	}
	return(result);
}

/****************************************************/

int 
sys_rmdir(call_t call, const char *pathname)
{
	WIN_NAMEIDATA wPath;
	int result = -1;

	if (!vfs_rmdir(path_win(&wPath, pathname, O_NOFOLLOW))){
		__errno_posix(call.Task, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
int 
sys_linkat(call_t call, int fd1, const char *name1, int fd2, const char *name2, int flag)
{
	WIN_NAMEIDATA wpOld;
	WIN_NAMEIDATA wpNew;
	int result = -1;

	if (!vfs_link(pathat_win(&wpOld, fd1, name1, flag), pathat_win(&wpNew, fd2, name2, flag))){
		__errno_posix(call.Task, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
int 
sys_link(call_t call, const char *oldpath, const char *newpath)
{
	return(sys_linkat(call, AT_FDCWD, oldpath, AT_FDCWD, newpath, AT_SYMLINK_NOFOLLOW));
}
int 
__unlinkat(WIN_TASK *Task, int dirfd, const char *pathname, int flags)
{
	WIN_NAMEIDATA wPath;
	int result = -1;
	BOOL bResult;

	if (flags & AT_REMOVEDIR){
		bResult = vfs_rmdir(pathat_win(&wPath, dirfd, pathname, AT_SYMLINK_NOFOLLOW));
	}else{
		bResult = vfs_unlink(pathat_win(&wPath, dirfd, pathname, AT_SYMLINK_NOFOLLOW));
	}
	if (!bResult){
		__errno_posix(Task, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
int 
sys_unlinkat(call_t call, int dirfd, const char *pathname, int flags)
{
	return(__unlinkat(call.Task, dirfd, pathname, flags));
}
int 
sys_unlink(call_t call, const char *pathname)
{
	return(__unlinkat(call.Task, AT_FDCWD, pathname, AT_SYMLINK_NOFOLLOW));
}
void 
sys_sync(call_t call)
{
	if (!vfs_sync()){
		__errno_posix(call.Task, GetLastError());
	}
}
int 
sys_fsync(call_t call, int fd)
{
	int result = -1;
	WIN_TASK *pwTask = call.Task;

	if (fd < 0 || fd >= OPEN_MAX){
		__errno_posix(pwTask, ERROR_INVALID_HANDLE);
	}else if (!vfs_fsync(&pwTask->Node[fd])){
		__errno_posix(call.Task, GetLastError());
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
	WIN_NAMEIDATA wPath;
	int result = -1;
	WIN_TASK *pwTask = call.Task;

	if (!vfs_chdir(pwTask, fdpath_win(&wPath, fildes, 0))){
		__errno_posix(pwTask, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
int 
sys_chdir(call_t call, const char *path)
{
	WIN_NAMEIDATA wPath;
	int result = -1;
	WIN_TASK *pwTask = call.Task;

	if (!vfs_chdir(pwTask, path_win(&wPath, path, 0))){
		__errno_posix(pwTask, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
int 
sys_chroot(call_t call, const char *path)
{
	int result = -1;
	WIN_NAMEIDATA wPath;

	if (!vfs_chroot(path_win(&wPath, path, 0))){
		__errno_posix(call.Task, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
ssize_t 
__readlinkat(WIN_TASK *Task, int dirfd, const char *pathname, char *buf, size_t bufsiz)
{
	WIN_NAMEIDATA wpePath;
	ssize_t result = -1;

	if (!vfs_readlink(pathat_win(&wpePath, dirfd, pathname, AT_SYMLINK_NOFOLLOW), FALSE)){
		__errno_posix(Task, GetLastError());
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
__symlinkat(WIN_TASK *Task, const char *oldpath, int newdirfd, const char *newpath)
{
	WIN_NAMEIDATA wpOld;
	WIN_NAMEIDATA wpNew;
	WIN_VATTR wStat = {0};
	WIN_MODE wMode;
	int result = -1;

	pathat_win(&wpOld, 0, oldpath, AT_SYMLINK_FOLLOW);
//	path_win(&wpOld, oldpath, 0);
	if (wpOld.Attribs != -1){
		vfs_stat(&wpOld, &wStat);
	}
	if (!vfs_symlink(&wpOld, &wStat, mode_win(&wMode, 0755), pathat_win(&wpNew, newdirfd, newpath, AT_SYMLINK_NOFOLLOW))){
		__errno_posix(Task, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
int 
sys_symlinkat(call_t call, const char *oldpath, int newdirfd, const char *newpath)
{
	return(__symlinkat(call.Task, oldpath, newdirfd, newpath));
}
int 
sys_symlink(call_t call, const char *oldpath, const char *newpath)
{
	return(__symlinkat(call.Task, oldpath, AT_FDCWD, newpath));
}
int 
__faccessat(WIN_TASK *Task, int dirfd, const char *pathname, int mode, int flags)
{
	WIN_NAMEIDATA wPath;
	ACCESS_MASK amDesired = 0;
	int result = -1;

	if (mode & R_OK){
		amDesired |= WIN_S_IREAD;
	}
	if (mode & W_OK){
		amDesired |= WIN_S_IWRITE;
	}
	if (mode & X_OK){
		amDesired |= WIN_S_IEXEC;
	}
	if (!vfs_access(pathat_win(&wPath, dirfd, pathname, flags), amDesired)){
		__errno_posix(Task, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
int 
sys_faccessat(call_t call, int dirfd, const char *pathname, int mode, int flags)
{
	return(__faccessat(call.Task, dirfd, pathname, mode, flags));
}
int 
sys_access(call_t call, const char *pathname, int mode)
{
	return(__faccessat(call.Task, AT_FDCWD, pathname, mode, AT_SYMLINK_FOLLOW));
}
int 
__renameat(WIN_TASK *Task, int dirfd, const char *path, int newdirfd, const char *newpath)
{
	WIN_NAMEIDATA wpOld;
	WIN_NAMEIDATA wpNew;
	int result = -1;

	if (pathat_win(&wpOld, dirfd, path, AT_SYMLINK_NOFOLLOW)->Attribs == -1){
		__errno_posix(Task, GetLastError());
	}else if (!vfs_rename(&wpOld, pathat_win(&wpNew, newdirfd, newpath, AT_SYMLINK_NOFOLLOW))){
		__errno_posix(Task, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
int 
sys_renameat(call_t call, int dirfd, const char *path, int newdirfd, const char *newpath)
{
	return(__renameat(call.Task, dirfd, path, newdirfd, newpath));
}
int 
sys_rename(call_t call, const char *path, const char *newpath)
{
	return(__renameat(call.Task, AT_FDCWD, path, AT_FDCWD, newpath));
}
int 
sys_ftruncate(call_t call, int fd, off_t length)
{
	int result = -1;
	WIN_TASK *pwTask = call.Task;

	if (fd < 0 || fd >= OPEN_MAX){
		__errno_posix(pwTask, ERROR_INVALID_HANDLE);
	}else if (!vfs_ftruncate(&pwTask->Node[fd], length)){
		__errno_posix(pwTask, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
int 
sys_truncate(call_t call, const char *path, off_t length)
{
	int result = -1;
	WIN_NAMEIDATA wPath;
	WIN_FLAGS wFlags;

	if (!vfs_truncate(path_win(&wPath, path, 0), flags_win(&wFlags, O_WRONLY), length)){
		__errno_posix(call.Task, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}

/****************************************************/

int 
sys_dup(call_t call, int oldfd)
{
	int result = -1;
	WIN_VNODE vnResult = {0};
	WIN_TASK *pwTask = call.Task;

	if (oldfd < 0 || oldfd >= OPEN_MAX){
		__errno_posix(pwTask, ERROR_INVALID_HANDLE);
	}else if (!vfs_dup(&pwTask->Node[oldfd], &vnResult)){
		__errno_posix(pwTask, GetLastError());
	}else{
		result = fd_posix(pwTask, &vnResult, 0);
	}
	return(result);
}
int 
sys_dup2(call_t call, int oldfd, int newfd)
{
	int result = -1;
	WIN_TASK *pwTask = call.Task;

	if (oldfd < 0 || oldfd >= OPEN_MAX){
		__errno_posix(pwTask, ERROR_INVALID_HANDLE);
	}else if (newfd < 0 || newfd >= OPEN_MAX){
		__errno_posix(pwTask, ERROR_BAD_ARGUMENTS);
	}else if (!vfs_dup2(&pwTask->Node[oldfd], &pwTask->Node[newfd])){
		__errno_posix(pwTask, GetLastError());
	}else{
		result = newfd;
	}
	return(result);
}
off_t 
sys_lseek(call_t call, int fd, off_t offset, int whence)
{
	off_t result = -1;
	DWORD dwMethod = FILE_BEGIN;
	LONGLONG llResult;
	LONGLONG llOffset = offset;		/* __int64_t */
	WIN_TASK *pwTask = call.Task;

	if (whence == SEEK_CUR){
		dwMethod = FILE_CURRENT;
	}else if (whence == SEEK_END){
		dwMethod = FILE_END;
	}
	if (fd < 0 || fd >= OPEN_MAX){
		__errno_posix(pwTask, ERROR_INVALID_HANDLE);
	}else if (!vfs_lseek(&pwTask->Node[fd], llOffset, dwMethod, &llResult)){
		__errno_posix(pwTask, GetLastError());
	}else{
		result = llResult;
	}
	return(result);
}
int 
sys_close(call_t call, int fd)
{
	int result = -1;
	WIN_TASK *pwTask = call.Task;

	if (fd < 0 || fd >= OPEN_MAX){
		__errno_posix(pwTask, ERROR_INVALID_HANDLE);
	}else if (!vfs_close(&pwTask->Node[fd])){
		__errno_posix(pwTask, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
int 
__closefrom(WIN_VNODE List[], int offset)
{
	WIN_VNODE *pvNode = &List[offset];

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
	int result = -1;

	if (fd < 0 || fd >= OPEN_MAX){
		__errno_posix(call.Task, ERROR_INVALID_HANDLE);
	}else{
		result = __closefrom(call.Task->Node, fd);
	}
	return(result);
}
ssize_t 
sys_read(call_t call, int fd, void *buf, size_t count)
{
	DWORD dwResult = -1;
	DWORD dwCount = count;
	WIN_TASK *pwTask = call.Task;
	CHAR szMessage[MAX_MESSAGE];

	if (pwTask->TracePoints & KTRFAC_USER){
		ktrace_USER(pwTask, "VNODE", szMessage, vfs_ktrace(&pwTask->Node[fd], szMessage));
	}
	if (fd < 0 || fd >= OPEN_MAX){
		__errno_posix(pwTask, ERROR_INVALID_HANDLE);
	}else if (!vfs_read(&pwTask->Node[fd], buf, dwCount, &dwResult)){
		__errno_posix(pwTask, GetLastError());
	}else if (pwTask->TracePoints & KTRFAC_GENIO){
		ktrace_GENIO(pwTask, fd, UIO_READ, buf, dwResult);
	}
	return((ssize_t)dwResult);
}
ssize_t 
sys_write(call_t call, int fd, const void *buf, size_t nbyte)
{
	DWORD dwResult = -1;
	DWORD dwCount = nbyte;
	WIN_TASK *pwTask = call.Task;
	CHAR szMessage[MAX_MESSAGE];

	if (pwTask->TracePoints & KTRFAC_GENIO){
		ktrace_GENIO(pwTask, fd, UIO_WRITE, buf, nbyte);
	}
	if (pwTask->TracePoints & KTRFAC_USER){
		ktrace_USER(pwTask, "VNODE", szMessage, vfs_ktrace(&pwTask->Node[fd], szMessage));
	}
	if (fd < 0 || fd >= OPEN_MAX){
		__errno_posix(pwTask, ERROR_INVALID_HANDLE);
	}else if (!vfs_write(&pwTask->Node[fd], buf, dwCount, &dwResult)){
		__errno_posix(pwTask, GetLastError());
	}
	return((ssize_t)dwResult);
}
ssize_t 
sys_pread(call_t call, int fd, void *buf, size_t nbyte, off_t offset)
{
	DWORD dwResult = -1;
	DWORD dwCount = nbyte;
	WIN_TASK *pwTask = call.Task;

	if (offset < 0){
		__errno_posix(pwTask, ERROR_BAD_ARGUMENTS);
	}else if (fd < 0 || fd >= OPEN_MAX){
		__errno_posix(pwTask, ERROR_INVALID_HANDLE);
	}else if (!vfs_pread(&pwTask->Node[fd], buf, dwCount, offset, &dwResult)){
		__errno_posix(call.Task, GetLastError());
	}
	return((ssize_t)dwResult);
}
ssize_t 
sys_pwrite(call_t call, int fd, const void *buf, size_t nbyte, off_t offset)
{
	DWORD dwResult = -1;
	DWORD dwCount = nbyte;
	WIN_TASK *pwTask = call.Task;

	if (offset < 0){
		__errno_posix(pwTask, ERROR_BAD_ARGUMENTS);
	}else if (fd < 0 || fd >= OPEN_MAX){
		__errno_posix(pwTask, ERROR_INVALID_HANDLE);
	}else if (!vfs_pwrite(&pwTask->Node[fd], buf, dwCount, offset, &dwResult)){
		__errno_posix(pwTask, GetLastError());
	}
	return((ssize_t)dwResult);
}

/****************************************************/

int 
sys_getthrid(call_t call)
{
	return(call.Task->TaskId);	/* git.exe */
}
pid_t 
__getpgid(WIN_TASK *Task, pid_t pid)
{
	pid_t result = -1;

	if (pid < 0){
		__errno_posix(Task, ERROR_BAD_ARGUMENTS);
	}else if (pid >= CHILD_MAX){
		__errno_posix(Task, ERROR_MAX_THRDS_REACHED);
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
	int result = -1;

	if (pid < 0){
		__errno_posix(call.Task, ERROR_BAD_ARGUMENTS);
	}else if (pid >= CHILD_MAX){
		__errno_posix(call.Task, ERROR_MAX_THRDS_REACHED);
	}else{
		result = 0;
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
	int result = -1;
	WIN_NAMEIDATA wPath;
	WIN_FLAGS wFlags = {GENERIC_READ + GENERIC_WRITE, 
		FILE_SHARE_READ, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 0};
	WIN_MODE wMode = {0};
	WIN_VNODE vNode = {0};

	if (!vfs_open(path_win(&wPath, path, 0), &wFlags, mode_win(&wMode, 0700), &vNode)){
		__errno_posix(call.Task, GetLastError());
	}else if (!vfs_revoke(&vNode)){
		__errno_posix(call.Task, GetLastError());
	}else if (vfs_close(&vNode)){
		result = 0;
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
	int result = -1;

	if (scale && !samples){
		__errno_posix(call.Task, ERROR_INVALID_ADDRESS);
	}else{
		result = 0;
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
	pid_t result = -1;
	DWORD dwResult = 0;

	if (!win___tfork_thread(NULL, psize, (PVOID)startfunc, startarg, &dwResult)){
		__errno_posix(&__Tasks[CURRENT], GetLastError());
	}else{
		result = dwResult;
	}	
}
