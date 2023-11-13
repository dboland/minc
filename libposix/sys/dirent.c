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

#include <sys/dirent.h>

/****************************************************/

void *
dirent_posix(struct dirent *ent, WIN_DIRENT *Entity)
{
	switch (Entity->FileType){
		case WIN_VDIR:
			ent->d_type = DT_DIR;
			break;
		case WIN_VLNK:
			ent->d_type = DT_LNK;
			break;
		case WIN_VCHR:
			ent->d_type = DT_CHR;
			break;
		case WIN_VSOCK:
			ent->d_type = DT_SOCK;
			break;
		case WIN_VBLK:
			ent->d_type = DT_BLK;
			break;
		default:
			ent->d_type = DT_REG;
	}
//	ent->d_namlen = win_stpcpy(ent->d_name, Entity->FileName) - ent->d_name;
//	ent->d_reclen = DIRENT_RECSIZE(ent->d_namlen);
//	ent->d_off = ent->d_reclen + ent->d_namlen;
	ent->d_namlen = win_wcstombs(ent->d_name, Entity->FileName, MAXNAMLEN);
	ent->d_reclen = sizeof(struct dirent);
	ent->d_off = (off_t)ent->d_reclen;	/* 64 bit! */
	ent->d_fileno = (ino_t)Entity->Index;
	return(ent + 1);
}

/****************************************************/

int
sys_getdents(call_t call, int fd, void *buf, size_t nbytes)
{
	int result = 0;
	DWORD dwCount = nbytes / sizeof(struct dirent);
	PVOID pvData = win_malloc(dwCount * sizeof(WIN_DIRENT));
	WIN_DIRENT *pEntity = pvData;
	WIN_TASK *pwTask = call.Task;

	if (fd < 0 || fd >= OPEN_MAX){
		result = -EINVAL;
	}else if (!vfs_getdents(&pwTask->Node[fd], pEntity, dwCount, &dwCount)){
		result -= errno_posix(GetLastError());
	}else while (dwCount--){
		buf = dirent_posix(buf, pEntity);
		result += sizeof(struct dirent);
		pEntity++;
	}
	win_free(pvData);
	return(result);
}
