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

/**************************************************************/

VOID 
LinkReadTarget(HANDLE Handle)
{
	WORD IDListSize;
	DWORD dwSize;

	ReadFile(Handle, &IDListSize, sizeof(IDListSize), &dwSize, NULL);
	SetFilePointer(Handle, IDListSize, NULL, FILE_CURRENT);
}
VOID 
LinkReadInfoNetwork(COMMON_NETWORK_RELATIVE_LINK *Info, LPSTR Buffer)
{
	BYTE *Data = (BYTE *)Info;

	win_strcpy(win_stpcpy(Buffer, &Data[Info->DeviceNameOffset]), "\\");
}
VOID 
LinkReadInfo(HANDLE Handle, LPSTR Buffer)
{
	LINK_INFO *Info;
	DWORD LinkInfoSize;
	BYTE *Data;
	DWORD dwSize;

	ReadFile(Handle, &LinkInfoSize, sizeof(LinkInfoSize), &dwSize, NULL);
	Data = LocalAlloc(LMEM_FIXED, LinkInfoSize);
	Info = (LINK_INFO *)Data;
	ReadFile(Handle, &Data[4], LinkInfoSize - 4, &dwSize, NULL);
	if (Info->LinkInfoFlags & VolumeIDAndLocalBasePath){
		win_strcpy(Buffer, &Data[Info->LocalBasePathOffset]);
	}
	if (Info->LinkInfoFlags & CommonNetworkRelativeLinkAndPathSuffix){
		CHAR *NetInfo = &Data[Info->CommonNetworkRelativeLinkOffset];
		LinkReadInfoNetwork((COMMON_NETWORK_RELATIVE_LINK *)NetInfo, Buffer);
		win_strcat(Buffer, &Data[Info->CommonPathSuffixOffset]);
	}
	LocalFree(Data);
}
VOLUME_ID *
LinkCreateInfoVolume(WIN_VATTR *Stat)
{
	VOLUME_ID *Info;
	DWORD dwSize = sizeof(VOLUME_ID);
	BYTE *Data;

	dwSize += win_strlen(Stat->VolumeLabel) + 1;
	Info = LocalAlloc(LPTR, dwSize);
	Data = (BYTE *)Info;
	Info->VolumeIDSize = dwSize;
	Info->DriveType = DRIVE_FIXED;
	Info->DriveSerialNumber = Stat->VolumeSerialNumber;
	Info->VolumeLabelOffset = sizeof(VOLUME_ID);
	win_strcpy(&Data[Info->VolumeLabelOffset], Stat->VolumeLabel);
	return(Info);
}
VOID 
LinkCreateInfo(HANDLE Handle, LPCSTR FileName, WIN_VATTR *Stat)
{
	LINK_INFO lInfo = {0};
	VOLUME_ID *vInfo = LinkCreateInfoVolume(Stat);
	DWORD dwCount;

	lInfo.LinkInfoSize = sizeof(LINK_INFO) + vInfo->VolumeIDSize + win_strlen(FileName) + 1;
	lInfo.LinkInfoHeaderSize = sizeof(LINK_INFO);
	lInfo.LinkInfoFlags = VolumeIDAndLocalBasePath;
	lInfo.VolumeIDOffset = sizeof(LINK_INFO);
	lInfo.LocalBasePathOffset = lInfo.VolumeIDOffset + vInfo->VolumeIDSize;
	WriteFile(Handle, &lInfo, sizeof(LINK_INFO), &dwCount, NULL);
	WriteFile(Handle, vInfo, vInfo->VolumeIDSize, &dwCount, NULL);
	WriteFile(Handle, FileName, win_strlen(FileName) + 1, &dwCount, NULL);
	LocalFree(vInfo);
}

/**************************************************************/

link_rename(WIN_NAMEIDATA *Path, WIN_NAMEIDATA *Result)
{
	BOOL bResult = FALSE;

	win_wcscpy(Result->R, L".lnk");
	if (Result->Attribs == -1){
		bResult = MoveFileW(Path->Resolved, Result->Resolved);
	}else if (Result->FileType != WIN_VLNK){
		SetLastError(ERROR_FILE_EXISTS);
	}else if (DeleteFileW(Result->Resolved)){
		bResult = MoveFileExW(Path->Resolved, Result->Resolved, MOVEFILE_COPY_ALLOWED);
	}
	return(bResult);
}
