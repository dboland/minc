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

#include <sys/mount.h>

/****************************************************/

char *
fstype_posix(char *type, void *options, LPWSTR TypeName)
{
	if (!win_wcsncmp(TypeName, L"FAT", 3)){
		struct msdosfs_args *msdos = options;
		win_strncpy(type, "msdos", MFSNAMELEN);
		msdos->uid = rid_posix(&SidEveryone);
		msdos->gid = rid_posix(&SidEveryone);
		msdos->mask = 00777;
	}else if (!win_wcscmp(TypeName, L"ISO9660")){
		win_strncpy(type, "cd9660", MFSNAMELEN);
	}else{
		win_wcstombs(type, win_wcslcase(TypeName), MFSNAMELEN);
	}
	return(type);
}
u_int32_t 
fsflags_posix(DWORD Flags)
{
	u_int32_t result = 0;

	if (Flags & FILE_READ_ONLY_VOLUME){
		result |= MNT_RDONLY;
	}
	if (!(Flags & FILE_PERSISTENT_ACLS)){
		result |= MNT_NOSUID;
	}
	if (Flags & FILE_VOLUME_QUOTAS){
		result |= MNT_QUOTA;
	}
	return(result);
}
struct statfs *
statfs_posix(struct statfs *buf, WIN_STATFS *Stat)
{
	WIN_DEVICE *pDevice = DEVICE(Stat->DeviceId);

	win_bzero(buf, sizeof(struct statfs));

	buf->f_flags = fsflags_posix(Stat->Flags);
	buf->f_namemax = Stat->MaxPath;
	buf->f_bsize = Stat->BytesPerSector * Stat->SectorsPerCluster;
	buf->f_blocks = Stat->ClustersTotal;
	buf->f_bfree = Stat->FreeClusters;
	buf->f_bavail = Stat->FreeClusters;

	buf->f_fsid.val[0] = pDevice->DeviceId >> 8;
	buf->f_fsid.val[1] = pDevice->DeviceId & 0xFF;
	buf->f_ctime = time_posix(&Stat->MountTime);

	/* fs type name and options */
	fstype_posix(buf->f_fstypename, &buf->mount_info, Stat->TypeName);

	/* directory on which mounted */
	pathn_posix(buf->f_mntonname, win_wcslcase((LPWSTR)Stat->Drive), MNAMELEN);

	/* mounted file system */
//	win_strncpy(win_stpcpy(buf->f_mntfromname, "/dev/"), win_strlcase(pDevice->Name), MNAMELEN - 5);
	win_strncpy(win_stpcpy(buf->f_mntfromname, "/dev/"), pDevice->Name, MNAMELEN - 5);

	/* special for mount request */
	win_strncpy(buf->f_mntfromspec, buf->f_mntfromname, MNAMELEN);

	return(buf + 1);
}

/****************************************************/

int
mount_NTFS(WIN_TASK *Task, const char *dir, struct ntfs_args *args)
{
	int result = -1;
	WIN_NAMEIDATA wPath;
	WIN_MODE wMode;
	WIN_VATTR wAttr;

	if (!vfs_stat(path_win(&wPath, args->fspec, O_NOFOLLOW), &wAttr)){
		__errno_posix(Task, GetLastError());
	}else if (!drive_mount(path_win(&wPath, dir, O_NOCROSS), &wAttr, mode_win(&wMode, args->mode))){
		__errno_posix(Task, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
int
mount_MSDOS(WIN_TASK *Task, const char *dir, struct msdosfs_args *args)
{
	int result = -1;
	WIN_NAMEIDATA wPath;
	WIN_MODE wMode;
	WIN_VATTR wAttr;

	if (!vfs_stat(path_win(&wPath, args->fspec, O_NOFOLLOW), &wAttr)){
		__errno_posix(Task, GetLastError());
	}else if (!drive_mount(path_win(&wPath, dir, O_NOCROSS), &wAttr, mode_win(&wMode, args->mask))){
		__errno_posix(Task, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
int
mount_CD9660(WIN_TASK *Task, const char *dir, struct iso_args *args)
{
	int result = -1;
	WIN_NAMEIDATA wPath;
	WIN_MODE wMode;
	WIN_VATTR wAttr;

	if (!vfs_stat(path_win(&wPath, args->fspec, O_NOFOLLOW), &wAttr)){
		__errno_posix(Task, GetLastError());
	}else if (!drive_mount(path_win(&wPath, dir, O_NOCROSS), &wAttr, mode_win(&wMode, 0666))){
		__errno_posix(Task, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}

/****************************************************/

int 
sys_mount(call_t call, const char *type, const char *dir, int flags, void *data)
{
	int result = -1;
	WIN_TASK *pwTask = call.Task;

	if (!win_strcmp(type, MOUNT_NTFS)){
		result = mount_NTFS(pwTask, dir, (struct ntfs_args *)data);

	}else if (!win_strcmp(type, MOUNT_MSDOS)){
		result = mount_MSDOS(pwTask, dir, (struct msdosfs_args *)data);

	}else if (!win_strcmp(type, MOUNT_CD9660)){
		result = mount_CD9660(pwTask, dir, (struct iso_args *)data);

	}else{
		__errno_posix(pwTask, ERROR_NOT_SUPPORTED);

	}
	return(result);
}
int
sys_unmount(call_t call, const char *dir, int flags)
{
	int result = -1;
	WIN_NAMEIDATA wPath;

	if (!drive_unmount(path_win(&wPath, dir, O_NOCROSS))){
		__errno_posix(call.Task, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
int 
sys_statfs(call_t call, const char *path, struct statfs *buf)
{
	int result = -1;
	WIN_STATFS fsInfo = {0};
	WIN_NAMEIDATA wPath;

	if (!drive_statfs(path_win(&wPath, path, 0), &fsInfo)){
		__errno_posix(call.Task, GetLastError());
	}else{
		statfs_posix(buf, &fsInfo);
		result = 0;
	}
	return(result);
}
int 
sys_fstatfs(call_t call, int fd, struct statfs *buf)
{
	int result = -1;
	WIN_STATFS fsInfo = {0};
	WIN_NAMEIDATA wPath;
	WIN_TASK *pwTask = call.Task;

	if (!drive_statfs(fdpath_win(&wPath, fd, 0), &fsInfo)){
		__errno_posix(pwTask, GetLastError());
	}else{
		statfs_posix(buf, &fsInfo);
		result = 0;
	}
	return(result);
}
int 
sys_getfsstat(call_t call, struct statfs *buf, size_t bufsize, int flags)
{
	int result = 0;
	WIN_STATFS fsInfo;
	DWORD dwFlags = 0;
	DWORD dwIndex = 1;
	WIN_MOUNT *pwMount = &__Mounts[dwIndex];

	if (flags == MNT_NOWAIT){
		dwFlags |= WIN_MNT_NOWAIT;
	}
	while (dwIndex < WIN_MOUNT_MAX){
		if (pwMount->DeviceId){
			if (buf){
				drive_getfsstat(pwMount, dwFlags, &fsInfo);
				buf = statfs_posix(buf, &fsInfo);
			}
			result++;
		}
		dwIndex++;
		pwMount++;
	}
	return(result);
}
