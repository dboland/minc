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

#include <ddk/ntddk.h>


/* Windows device descriptions:
 * /mnt/c/MinGW/include/ddk/ntdd*.h
 *
 * grep -r DEVICE_NAME /mnt/c/MinGW/include/ddk/
 * mode.com
 * mountvol.exe
 * devcon.exe (from support.cab in Windows Driver Kit [WDK])
 * msinfo32
 *
 * Physical Device Objects (PDO) represent individual devices on a bus. 
 * It is always at the bottom of the device stack. Other drivers for the
 * device attach on top of the PDO.
 *
 * All vendor device descriptions can be found in the Windows registry:
 * HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Enum
 *
 * "Volume{}" (FS_TYPE_VOLUME) are in HKEY_LOCAL_MACHINE\SYSTEM\MountedDevices
 * which contain the FDC#, STORAGE#, IDE# types. They can be accessed
 * as directories:
 * \\.\Volume{82e82774-791f-11d7-a4cf-806d6172696f} maps to \Device\HarddiskVolume1
 *
 */

/************************************************************/

DWORD 
VfsLinkType(LPWSTR BusName)
{
	DWORD dwType = FS_TYPE_PROCESS;

	if (!*BusName){
		dwType = FS_TYPE_LINK;
		win_wcscpy(BusName, L"LINK");
	}else if (!win_wcscmp(BusName, L"Volume")){
		dwType = FS_TYPE_VOLUME;
	}else if (!win_wcsncmp(BusName, L"NP", 2)){
		dwType = FS_TYPE_NPF;
	}
	return(dwType);
}
DWORD 
VfsBusName(LPCWSTR DosPath, LPWSTR Result)
{
	DWORD dwType = FS_TYPE_PROCESS;
	LPCWSTR P = DosPath;
	WCHAR *B = Result;
	WCHAR C;

	ZeroMemory(Result, sizeof(WCHAR) * MAX_NAME);
	while (C = *P++){
		if (C == '#'){
			dwType = FS_TYPE_PDO;
			break;
		}else if (C == ':'){
			dwType = FS_TYPE_DRIVE;
			break;
		}else if (C == '{'){
			dwType = VfsLinkType(Result);
			break;
		}
		*B++ = C;
	}
	return(dwType);
}
DWORD 
VfsClassName(LPCWSTR NtPath, LPWSTR Result)
{
	LPCWSTR P = NtPath;
	DWORD dwDepth = 0;
	WCHAR C;

	while (C = *P++){
		if (C == '\\'){
			dwDepth++;
		}else if (dwDepth == 2){
			*Result++ = C;
		}
	}
	*Result = 0;
	return(dwDepth);
}
/* BOOL 
VfsBootTime(FILETIME *Result)
{
	BOOL bResult = FALSE;
	SYSTEM_TIMEOFDAY_INFORMATION todInfo;
	ULONG ulSize = sizeof(SYSTEM_TIMEOFDAY_INFORMATION);
	NTSTATUS ntStatus;

	ntStatus = NtQuerySystemInformation(SystemTimeOfDayInformation, &todInfo, ulSize, &ulSize);
	if (!NT_SUCCESS(ntStatus)){
		WIN_ERR("NtQuerySystemInformation(SystemTimeOfDayInformation): %s\n", nt_strerror(ntStatus));
	}else{
		Result->dwLowDateTime = todInfo.BootTime.LowPart;
		Result->dwHighDateTime = todInfo.BootTime.HighPart;
		bResult = TRUE;
	}
	return(bResult);
} */
LPCWSTR 
VfsPrevious(LPCWSTR Origin, LPCWSTR String)
{
	LPCWSTR psz = String - 1;

	do {
		String = psz--;
		if (String == Origin){
			break;
		}else if (String < Origin){
			return(NULL);
		}
	} while (*psz);
	return(String);
}

/************************************************************/

BOOL 
vfs_setvfs(WIN_CFDATA *Config, DWORD Flags)
{
	DWORD dwCount = MIN_BUFSIZE * 4;
	DWORD dwSize = 0;
	LPWSTR pszBuffer = win_malloc(dwCount * sizeof(WCHAR));

	ZeroMemory(Config, sizeof(WIN_CFDATA));
	while (!(dwSize = QueryDosDeviceW(NULL, pszBuffer, dwCount))){
		if (ERROR_INSUFFICIENT_BUFFER == GetLastError()){
			dwCount += MIN_BUFSIZE;
			pszBuffer = win_realloc(pszBuffer, dwCount * sizeof(WCHAR));
		}else{
			WIN_ERR("QueryDosDevice(%d): %s\n", dwCount, win_strerror(GetLastError()));
			win_free(pszBuffer);
			return(FALSE);
		}
	}
//	VfsBootTime(&Config->BootTime);
	if (Flags & WIN_MNT_REVERSED){
		Config->Next = VfsPrevious(pszBuffer, pszBuffer + dwSize - 1);
	}else{
		Config->Next = pszBuffer;
	}
	Config->Strings = pszBuffer;
	return(TRUE);
}
VOID 
vfs_endvfs(WIN_CFDATA *Config)
{
	win_free(Config->Strings);
}
BOOL 
vfs_getvfs(WIN_CFDATA *Config, DWORD Flags)
{
	BOOL bResult = FALSE;
	LPCWSTR pszNext = Config->Next;

	if (!pszNext || !*pszNext){
		SetLastError(ERROR_NO_MORE_ITEMS);
	}else if (!QueryDosDeviceW(pszNext, Config->NtPath, WIN_PATH_MAX)){
		WIN_ERR("QueryDosDevice(%ls): %s\n", pszNext, win_strerror(GetLastError()));
	}else{
		Config->FSType = VfsBusName(pszNext, Config->BusName);
		Config->Depth = VfsClassName(Config->NtPath, Config->ClassName);
		win_wcsucase(Config->BusName);
		Config->NtName = win_basename(Config->NtPath);
		Config->DosPath = pszNext;
		if (Flags & WIN_MNT_REVERSED){
			Config->Next = VfsPrevious(Config->Strings, pszNext);
		}else{
			Config->Next += win_wcslen(pszNext) + 1;
		}
		bResult = TRUE;
	}
	return(bResult);
}
