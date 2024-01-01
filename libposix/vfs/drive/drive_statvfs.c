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

#include <ddk/ntifs.h>		/* Installable File System */

#define MOUNTDEV_MOUNTED_DEVICE		L"{53f5630d-b6bf-11d0-94f2-00a0c91efb8b}"

/************************************************************/

DWORD 
DriveLookupClass(LPCWSTR ClassName)
{
	DWORD dwResult = DEV_CLASS_STORAGE;

	if (!win_wcsncmp(ClassName, L"Floppy", 6)){
		dwResult |= DEV_BUS_FDC;

//	}else if (!win_wcsncmp(ClassName, L"Harddisk", 8)){
//		dwResult |= DEV_BUS_USB;

	}else if (!win_wcscmp(ClassName, L"Ide")){
		dwResult |= DEV_BUS_IDE;

	}
	return(dwResult);
}
DWORD 
DriveLookupBus(LPCWSTR BusName)
{
	DWORD dwResult = DEV_CLASS_DISK;

	if (!win_wcsncmp(BusName, L"MINC", 4)){
		dwResult = DEV_TYPE_ROOT;

	}else if (!win_wcsncmp(BusName, L"SCSI", 4)){
		dwResult = DEV_TYPE_SCSI;

		/* Vista */

	}else if (!win_wcscmp(BusName, L"LOG")){
		dwResult = DEV_TYPE_PRINTK;

	}
	return(dwResult);
}
DWORD 
DriveLookup(LPCWSTR BusName, LPCWSTR ClassName, UINT DriveType)
{
	DWORD dwResult = 0;

	switch (DriveType){
		case DRIVE_REMOVABLE:
			dwResult = DriveLookupClass(ClassName);
			break;
		case DRIVE_NO_ROOT_DIR:		/* Not mounted (no drive letter) */
			dwResult = DriveLookupBus(BusName);
			break;
		case DRIVE_FIXED:
//			dwResult = DEV_TYPE_FIXED;
			dwResult = DEV_TYPE_VOLUME;
			break;
		case DRIVE_CDROM:
			dwResult = DEV_TYPE_CDROM;
			break;
		case DRIVE_REMOTE:
			dwResult = DEV_TYPE_REMOTE;
			break;
		case DRIVE_RAMDISK:
			dwResult = DEV_TYPE_RAMDISK;
			break;
		default:
			WIN_ERR("GetDriveType(%d): %s\n", DriveType, win_strerror(GetLastError()));
	}
	return(dwResult);
}

/****************************************************/

BOOL 
drive_statvfs(WIN_CFDATA *Config, DWORD Flags, WIN_CFDRIVER *Result)
{
	BOOL bResult = TRUE;
//	LPWSTR psz;
	UINT uiType = GetDriveTypeW(Config->DosPath);

	ZeroMemory(Result, sizeof(WIN_CFDRIVER));
	if (uiType != DRIVE_NO_ROOT_DIR){
		win_wcscpy(Result->ClassId, MOUNTDEV_MOUNTED_DEVICE);
	}
	win_wcscpy(win_wcpcpy(Result->Location, Config->NtPath), L"\\");
	Config->DeviceType = DriveLookup(Config->BusName, Config->ClassName, uiType);
//VfsDebugMount(Result, "drive_statvfs");
	return(bResult);
}
