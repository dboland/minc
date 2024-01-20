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

#include <sys/file.h>

/****************************************************/

void *
kfile_posix(struct kinfo_file *buf, WIN_TASK *Task, WIN_VNODE *Node)
{
	WIN_VATTR wStat = {0};
	struct stat info;
	WIN_VNODE vNode;
	WCHAR szBuffer[MAX_PATH];
	HANDLE hResult = NULL;
	wchar_t *command = win_basename(__Strings[Task->TaskId].Command);

	win_bzero(buf, sizeof(struct kinfo_file));
	win_memcpy(&vNode, Node, sizeof(WIN_VNODE));
	if (!(Node->Flags & HANDLE_FLAG_INHERIT)){
		if (hResult = win_F_INHERIT(Node->Handle, 0, Task->ProcessId)){
			vNode.Handle = hResult;
			vNode.Access = win_F_GETFL(hResult);
		}
	}
	buf->f_flag = FFLAGS(flflags_posix(&vNode));	/* SHORT: flags (see fcntl.h) */
	buf->f_type = DTYPE_VNODE;			/* INT: descriptor type */
	buf->v_type = vNode.FileType;			/* ENUM: vnode type */
	buf->p_pid = Task->TaskId;			/* PID_T: process id */
	buf->fd_fd = vNode.FileId;			/* INT: descriptor number */
	buf->v_tag = WIN_VT_NTFS;			/* ENUM: type of underlying data */
	buf->p_uid = rid_posix(&Task->UserSid);		/* UID_T: process credentials */
	buf->p_gid = rid_posix(&Task->GroupSid);	/* GID_T: process credentials */
	buf->p_tid = Task->ThreadId;			/* PID_T: thread id */
	win_wcstombs(buf->p_comm, command, KI_MAXCOMLEN);
	if (vfs_fstat(&vNode, &wStat)){
		stat_posix(Task, &info, &wStat);
		buf->va_fsid = info.st_dev;		/* DEV_T: filesystem device */
		buf->va_rdev = info.st_rdev;		/* DEV_T: raw device */
		buf->va_mode = info.st_mode;		/* MODE_T: file access mode and type */
		buf->va_fileid = info.st_ino;		/* LONG: file id */
		buf->f_uid = info.st_uid;		/* UID_T: descriptor credentials */
		buf->f_gid = info.st_gid;		/* GID_T: descriptor credentials */
		buf->va_size = info.st_size;
	}else{
//__PRINTF("path: %ls\n", win_F_GETPATH(vNode.Handle, szBuffer));
VfsDebugNode(&vNode, "kfile_posix");
	}
	return(buf + 1);
}

/****************************************************/

void *
file_KERN_FILE_BYPID(void *buf, size_t *size, WIN_TASK *Task, WIN_VNODE Node[])
{
	size_t result = 0;
	int fd = 0;

	while (fd < OPEN_MAX){
		if (Node->Access){
			if (buf){
				buf = kfile_posix(buf, Task, Node);
			}
			result += sizeof(struct kinfo_file);
		}
		Node++;
		fd++;
	}
	*size += result;
	return(buf);
}
int 
file_KERN_FILE(const int *name, void *buf, size_t *size)
{
	int result = 0;
	WIN_TASK *pwTask = &__Tasks[1];
	pid_t pid = 1;
	DWORD dwRemain;

	*size = 0;
	while (pid < CHILD_MAX){
		if (pwTask->Flags && pwTask->State != WIN_SZOMB){
			buf = file_KERN_FILE_BYPID(buf, size, pwTask, pwTask->Node);
		}
		pwTask++;
		pid++;
	}
	return(0);
}
