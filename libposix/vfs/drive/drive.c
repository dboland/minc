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

#include "drive_namei.c"
#include "drive_mount.c"
#include "drive_statvfs.c"

#define DEVINTERFACE_VOLUME		L"{53f5630d-b6bf-11d0-94f2-00a0c91efb8b}"

/****************************************************/

VOID 
drive_init(WIN_MOUNT *Mount, HINSTANCE Instance)
{
	/* mount first partition on boot disk
	 */
	GetModuleFileNameW(Instance, Mount->Path, MAX_PATH);
	win_dirname(win_dirname(Mount->Path));
	win_wcscpy(Mount->Drive, L"MINC:");
	Mount->FileType = WIN_VDIR;
	Mount->DeviceType = DEV_TYPE_ROOT;
	Mount->DeviceId = DEV_TYPE_ROOT;
	Mount->FSType = FS_TYPE_DISK;
//VfsDebugMount(Mount, "drive_init");
//	DefineDosDeviceW(DDD_RAW_TARGET_PATH, L"MINC:", L"\\Device\\HarddiskVolume1\\MinC");
}
WIN_DEVICE *
drive_match(LPCWSTR NtName, DWORD DeviceType)
{
	WIN_DEVICE *pwDevice = DEVICE(DeviceType);
	USHORT sClass = DeviceType & 0xFF00;
	USHORT sUnit = DeviceType & 0x00FF;

	while (sUnit < WIN_UNIT_MAX){
		if (!win_wcscmp(pwDevice->NtName, NtName)){
			break;
		}else if (!pwDevice->Flags){
			win_wcscpy(pwDevice->NtName, NtName);
			win_wcscpy(pwDevice->ClassId, DEVINTERFACE_VOLUME);
			pwDevice->DeviceType = DeviceType;
			pwDevice->DeviceId = sClass + sUnit;
			if (!dev_found(pwDevice)){
				msvc_printf("Warning: storage device %ls (type 0x%x) not configured\n", NtName, DeviceType);
			}
//VfsDebugDevice(pwDevice, "drive_match");
			break;
		}
		pwDevice++;
		sUnit++;
	}
	return(pwDevice);
}
