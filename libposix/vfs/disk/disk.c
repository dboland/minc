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

#include "disk_dirent.c"
#include "disk_syscall.c"
#include "link/link.c"
#include "file/file.c"
#include "directory/directory.c"
#include "disk_fcntl.c"
#include "disk_unistd.c"
#include "disk_namei.c"
#include "disk_stat.c"
#include "disk_time.c"
#include "disk_poll.c"

/****************************************************/

void 
disk_init(WIN_MOUNT *Root, HINSTANCE Instance)
{
	__Mounts = Root;

	/* Configure root/swap device. Originally done by
	 * diskconf() in OpenBSD.
	 */
	GetModuleFileNameW(Instance, Root->Path, MAX_PATH);
	win_dirname(win_dirname(Root->Path));
	win_wcscpy(Root->Drive, L"MINC:");
	Root->FileType = WIN_VDIR;
	Root->DeviceType = DEV_TYPE_ROOT;
	Root->DeviceId = DEV_TYPE_ROOT;
	Root->FSType = FS_TYPE_DISK;
//VfsDebugMount(Root, "disk_init");
//	DefineDosDeviceW(DDD_RAW_TARGET_PATH, L"MINC:", L"\\Device\\HarddiskVolume1\\MinC");
}
