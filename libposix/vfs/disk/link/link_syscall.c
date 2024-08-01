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

/**************************************************************/

VOLUME_ID *
LinkCreateVolumeInfo(LPCWSTR RealName)
{
	VOLUME_ID *Info;
	DWORD dwSize = sizeof(VOLUME_ID);
	BYTE *Data;
	WCHAR szDrive[MAX_NAME];
	WIN_MOUNT wMount = {0};

	if (RealName[1] == '\\'){
		RealName = L"ROOT:";
	}
	DriveStatVolume(win_drivename(szDrive, RealName), &wMount);
	dwSize += win_wcslen(wMount.Label) + 1;
	Info = LocalAlloc(LPTR, dwSize);
	Data = (BYTE *)Info;
	Info->VolumeIDSize = dwSize;
	Info->DriveType = GetDriveTypeW(szDrive);
	Info->DriveSerialNumber = wMount.Serial;
	Info->VolumeLabelOffset = sizeof(VOLUME_ID);
	win_wcstombs(&Data[Info->VolumeLabelOffset], wMount.Label, MAX_NAME);
	return(Info);
}
VOID 
LinkCreateInfo(HANDLE Handle, LPCWSTR Target, LPCWSTR RealName)
{
	LINK_INFO lInfo = {0};
	VOLUME_ID *pvInfo = LinkCreateVolumeInfo(RealName);
	CHAR szTarget[MAX_PATH];
	DWORD dwSize = win_wcstombs(szTarget, Target, MAX_PATH);
	DWORD dwResult;

//__PRINTF("szTarget: %s\n", szTarget);
	lInfo.LinkInfoSize = sizeof(LINK_INFO) + pvInfo->VolumeIDSize + dwSize + 1;
	lInfo.LinkInfoHeaderSize = sizeof(LINK_INFO);
	lInfo.LinkInfoFlags = VolumeIDAndLocalBasePath;
	lInfo.VolumeIDOffset = sizeof(LINK_INFO);
	lInfo.LocalBasePathOffset = lInfo.VolumeIDOffset + pvInfo->VolumeIDSize;
	WriteFile(Handle, &lInfo, sizeof(LINK_INFO), &dwResult, NULL);
	WriteFile(Handle, pvInfo, pvInfo->VolumeIDSize, &dwResult, NULL);
	WriteFile(Handle, szTarget, dwSize + 1, &dwResult, NULL);
	LocalFree(pvInfo);
}
VOID 
LinkCreateFile(HANDLE Handle, WIN_NAMEIDATA *Target, LPCWSTR PathName)
{
	WIN32_FILE_ATTRIBUTE_DATA faData = {0};
	WCHAR szRealName[MAX_PATH], *pszRealName = szRealName;
	SHELL_LINK_HEADER slHeader;
	DWORD dwSize = sizeof(SHELL_LINK_HEADER);

	if (Target->Attribs == -1){
		pszRealName = win_basename(win_wcscpy(szRealName, PathName));
	}
	win_wcscpy(pszRealName, Target->Resolved);
//__PRINTF("szRealName: %ls\n", szRealName);
	slHeader.HeaderSize = sizeof(SHELL_LINK_HEADER);
	slHeader.LinkCLSID = CLSID_ShellLink;
	slHeader.LinkFlags = HasLinkInfo;
	if (GetFileAttributesExW(szRealName, GetFileExInfoStandard, &faData)){
		slHeader.FileAttributes = faData.dwFileAttributes;
		slHeader.CreationTime = faData.ftCreationTime;
		slHeader.AccessTime = faData.ftLastAccessTime;
		slHeader.WriteTime = faData.ftLastWriteTime;
		slHeader.FileSize = faData.nFileSizeLow;
	}else{
		WIN_ERR("GetFileAttributesEx(%ls): %s\n", szRealName, win_strerror(GetLastError()));
	}
	WriteFile(Handle, &slHeader, dwSize, &dwSize, NULL);
	LinkCreateInfo(Handle, Target->Resolved, szRealName);
}
