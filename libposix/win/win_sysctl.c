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

UINT 
win_HW_PAGESIZE(VOID)
{
	SYSTEM_INFO sInfo;

	GetSystemInfo(&sInfo);
	return(sInfo.dwPageSize);
}
DWORD 
win_HW_NCPU(VOID)
{
	/* This information can be retreived with GetSystemInfo(),
	 * but is not wise to use. top.exe will want to get info per CPU,
	 * which will hang.
	 */
	return(1);
}
UINT 
win_HW_USERMEM(VOID)
{
	SYSTEM_INFO sInfo;

	GetSystemInfo(&sInfo);
	return(sInfo.lpMaximumApplicationAddress - sInfo.lpMinimumApplicationAddress);
}
UINT 
win_HW_PHYSMEM(VOID)
{
	MEMORYSTATUS msInfo = {sizeof(MEMORYSTATUS), 0};

	GlobalMemoryStatus(&msInfo);
	return(msInfo.dwTotalPhys);
}
DWORDLONG 
win_HW_PHYSMEM64(VOID)
{
	MEMORYSTATUSEX msInfo = {sizeof(MEMORYSTATUSEX), 0};

	GlobalMemoryStatusEx(&msInfo);
	return(msInfo.ullTotalPhys);
}

/****************************************************/

UINT 
win_KERN_CLOCKRATE(VOID)
{
	LARGE_INTEGER liFrequency = {0, 0};

	if (!QueryPerformanceFrequency(&liFrequency)){
		WIN_ERR("QueryPerformanceFrequency(): %s\n", win_strerror(GetLastError()));
	}
	return(liFrequency.LowPart);
}
BOOL 
win_KERN_PROC(DWORD ThreadId, WIN_KUSAGE *Result)
{
	BOOL bResult = FALSE;
	HANDLE hThread = NULL;

	if (!(hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, ThreadId))){
		return(FALSE);
	}else if (GetThreadTimes(hThread, &Result->Creation, &Result->Exit, &Result->Kernel, &Result->User)){
		bResult = CloseHandle(hThread);
	}else{
		WIN_ERR("GetThreadTimes(%d): %s\n", hThread, win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
win_KERN_HOSTNAME(LPSTR Current, LPCSTR New, DWORD Size)
{
	BOOL bResult = FALSE;

	if (Current){
		bResult = GetComputerNameEx(ComputerNameDnsFullyQualified, Current, &Size);
	}else if (New){
		bResult = SetComputerNameExA(ComputerNamePhysicalDnsHostname, New);
	}else{
		SetLastError(ERROR_BAD_ARGUMENTS);
	}
	return(bResult);
}
BOOL 
win_KERN_DOMAINNAME(LPSTR Current, LPCSTR New, DWORD Size)
{
	BOOL bResult = FALSE;

	if (Current){
		bResult = GetComputerNameEx(ComputerNameDnsDomain, Current, &Size);
	}else if (New){
		bResult = SetComputerNameExA(ComputerNamePhysicalDnsDomain, New);
	}else{
		SetLastError(ERROR_BAD_ARGUMENTS);
	}
	return(bResult);
}
BOOL 
win_KERN_CPTIME2(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION Buffer[], LONG Size)
{
	BOOL bResult = FALSE;
	NTSTATUS ntStatus;
	ULONG ulSize;

	ntStatus = NtQuerySystemInformation(SystemProcessorPerformanceInformation, Buffer, Size, &ulSize);
	if (!NT_SUCCESS(ntStatus)){
		WIN_ERR("NtQuerySystemInformation(SystemProcessorPerformanceInformation): %s\n", nt_strerror(ntStatus));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
win_KERN_BOOTTIME(FILETIME *Result)
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
}
