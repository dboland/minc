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

#include <ddk/ntifs.h>

/************************************************************/

BOOL 
DriveStatVolume(LPCWSTR Volume, WIN_STATFS *Result)
{
	BOOL bResult = FALSE;

	if (!GetVolumeInformationW(Volume, Result->Label, MAX_LABEL, &Result->Serial, 
		&Result->MaxPath, &Result->Flags, Result->TypeName, MAX_LABEL)){
//		WIN_ERR("GetVolumeInformation(%ls): %s\n", Volume, win_strerror(GetLastError()));
		return(FALSE);
	}else if (!GetDiskFreeSpaceW(Volume, &Result->SectorsPerCluster, 
		&Result->BytesPerSector, &Result->FreeClusters, &Result->ClustersTotal)){
		WIN_ERR("GetDiskFreeSpace(%ls): %s\n", Volume, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}

/************************************************************/

BOOL 
drive_getfsstat(WIN_MOUNT *Mount, DWORD Flags, WIN_STATFS *Result)
{
	BOOL bResult = TRUE;

	ZeroMemory(Result, sizeof(WIN_STATFS));
	win_wcscpy(Result->Drive, Mount->Drive);
	Result->DeviceId = Mount->DeviceId;
	Result->MountTime = Mount->Time;
	if (!(Flags & WIN_MNT_NOWAIT)){
		bResult = DriveStatVolume(Mount->Volume, Result);
	}else switch (Mount->DeviceType){
		case DEV_TYPE_CDROM:
			win_wcscpy(Result->TypeName, L"ISO9660");
			Result->Flags = FILE_READ_ONLY_VOLUME;
			break;
		case DEV_TYPE_FLOPPY:
			win_wcscpy(Result->TypeName, L"FAT");
			break;
		default:
			bResult = DriveStatVolume(Mount->Volume, Result);
	}
	return(bResult);
}
BOOL 
drive_statfs(WIN_NAMEIDATA *Path, WIN_STATFS *Result)
{
	BOOL bResult = FALSE;

	/* mount.exe -a
	 */
	ZeroMemory(Result, sizeof(WIN_STATFS));
//VfsDebugPath(Path, "drive_statfs");
	if (Path->Attribs == FILE_ATTRIBUTE_MOUNT){
		bResult = DriveStatVolume(Path->Resolved, Result);
	}else{
		bResult = DriveStatVolume(win_volname(Path->Resolved), Result);
	}
	return(bResult);
}
BOOL 
drive_mount(WIN_NAMEIDATA *Path, WIN_DEVICE *Device, WIN_MODE *Mode)
{
	BOOL bResult = FALSE;
	WIN_MOUNT *pwMount;
	LONG lMountId = MOUNTID(Path->Base[0]);

//VfsDebugPath(Path, "drive_mount");
	if (Path->Attribs == -1){
		return(FALSE);
	}else if (Path->FileType != WIN_VDIR){
		SetLastError(ERROR_DIRECTORY);
	}else if (Path->Attribs == FILE_ATTRIBUTE_MOUNT){
		SetLastError(ERROR_NOT_READY);
	}else if (!Device->Flags){
		SetLastError(ERROR_DEVICE_NOT_AVAILABLE);
	}else if (!SetFileAttributesW(Path->Resolved, FILE_ATTRIBUTE_SYSTEM)){
		WIN_ERR("SetFileAttributes(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else{
		pwMount = &__Mounts[lMountId];
		pwMount->Drive[0] = msvc_toupper(Path->Base[0]);
		pwMount->Drive[1] = ':';
		pwMount->Drive[2] = 0;
		pwMount->MountId = lMountId;
		pwMount->DeviceId = Device->DeviceId;
		pwMount->DeviceType = Device->DeviceType;
		pwMount->VolumeSerial = Device->Index;
		pwMount->FileType = WIN_VDIR;
		pwMount->FSType = Path->FSType;
		win_wcscpy(pwMount->Volume, Device->NtPath);
		GetSystemTimeAsFileTime(&pwMount->Time);
//VfsDebugMount(pwMount, "drive_mount");
//VfsDebugDevice(Device, "drive_mount");
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
drive_unmount(WIN_NAMEIDATA *Path)
{
	BOOL bResult = FALSE;

	if (Path->Attribs == -1){
		return(FALSE);
	}else if (Path->FileType != WIN_VDIR){
		SetLastError(ERROR_DIRECTORY);
	}else if (Path->Attribs != FILE_ATTRIBUTE_MOUNT){
		bResult = TRUE;
	}else if (!SetFileAttributesW(Path->Resolved, FILE_ATTRIBUTE_NORMAL)){
		WIN_ERR("SetFileAttributes(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else{
		ZeroMemory(&__Mounts[Path->MountId], sizeof(WIN_MOUNT));
		bResult = TRUE;
	}
	return(bResult);
}
