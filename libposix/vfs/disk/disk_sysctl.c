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

#include <winbase.h>

/****************************************************/

BOOL 
DiskStatVolume(LPCWSTR Volume, BOOL Verbose, LPWSTR Label, DWORD *Serial)
{
	BOOL bResult = FALSE;
	DWORD dwMaxPath, dwFlags;

	if (GetVolumeInformationW(Volume, Label, MAX_LABEL, Serial, &dwMaxPath, &dwFlags, NULL, 0)){
		bResult = TRUE;
	}else if (Verbose){
		WIN_ERR("GetVolumeInformation(%ls): %s\n", Volume, win_strerror(GetLastError()));
	}
	return(bResult);
}

/****************************************************/

BOOL 
disk_HW_DISKNAMES(WIN_DEVICE *Device, LPSTR Result)
{
	BOOL bResult = TRUE;
	WCHAR szLabel[MAX_LABEL];
	DWORD dwSerial = 0;
	WCHAR szVolume[MAX_PATH] = L"\\\\.\\GLOBALROOT";

	win_wcscat(szVolume, Device->NtPath);
	switch (Device->DeviceType){
		case DEV_TYPE_FIXED:
		case DEV_TYPE_REMOTE:
			bResult = DiskStatVolume(szVolume, FALSE, szLabel, &dwSerial);
			msvc_sprintf(Result, "%s:%lu", Device->Name, dwSerial);
			break;
		default:
			win_strcpy(Result, Device->Name);
	}
	return(bResult);
}
