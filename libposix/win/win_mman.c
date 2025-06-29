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

/************************************************************/

BOOL 
MManAlloc(PVOID Address, DWORD Size, DWORD Type, DWORD Protect, PVOID *Result)
{
	BOOL bResult = FALSE;
	PVOID pvResult = NULL;

	if (pvResult = VirtualAlloc(Address, Size, Type, Protect)){
		*Result = pvResult;
		bResult = TRUE;
	}else{
		WIN_ERR("VirtualAlloc(%d): %s\n", Size, win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
MManLoadFile(HANDLE Handle, PVOID Address, DWORD Size, LARGE_INTEGER *Offset, PVOID *Result)
{
	BOOL bResult = FALSE;
	DWORD dwResult;
	LARGE_INTEGER liCurrent = {0};

	if (!SetFilePointerEx(Handle, liCurrent, &liCurrent, FILE_CURRENT)){
		WIN_ERR("SetFilePointerEx(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else if (!SetFilePointerEx(Handle, *Offset, Offset, FILE_BEGIN)){
		WIN_ERR("SetFilePointerEx(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else if (!ReadFile(Handle, Address, Size, &dwResult, NULL)){
		WIN_ERR("MManLoadFile(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else if (!SetFilePointerEx(Handle, liCurrent, &liCurrent, FILE_BEGIN)){
		WIN_ERR("SetFilePointerEx(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else{
		*Result = Address;
		bResult = TRUE;
	}
	return(bResult);
}

/************************************************************/

BOOL 
win_mprotect(PVOID Address, SIZE_T Size, DWORD Protect)
{
	BOOL bResult = FALSE;
	DWORD dwProtect;

	if (!VirtualProtect(Address, Size, Protect, &dwProtect)){
		WIN_ERR("VirtualProtect(%d): %s\n", Address, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
win_mmap(HANDLE Handle, PVOID Address, SIZE_T Size, LARGE_INTEGER *Offset, DWORD Protect, PVOID *Result)
{
	BOOL bResult = FALSE;
	PVOID pvResult = NULL;

	if (Handle == INVALID_HANDLE_VALUE){
		bResult = MManAlloc(Address, Size, MEM_COMMIT, Protect, Result);
	}else if (Address){
		bResult = MManLoadFile(Handle, Address, Size, Offset, Result);
	}else if (!MManAlloc(NULL, Size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE, &pvResult)){
		return(FALSE);
	}else if (!MManLoadFile(Handle, pvResult, Size, Offset, Result)){
		VirtualFree(pvResult, 0, MEM_RELEASE);
	}else{
		bResult = win_mprotect(pvResult, Size, Protect);
	}
	return(bResult);
}
BOOL 
win_madvise(PVOID Address, SIZE_T Size, DWORD Type)
{
	BOOL bResult = FALSE;
	MEMORY_BASIC_INFORMATION mbInfo = {0};

	if (!Type){
		return(TRUE);
	}else if (!VirtualQuery(Address, &mbInfo, sizeof(MEMORY_BASIC_INFORMATION))){
		WIN_ERR("VirtualQuery(0x%x): %s\n", Address, win_strerror(GetLastError()));
	}else if (!VirtualAlloc(Address, Size, Type, mbInfo.AllocationProtect)){
		WIN_ERR("VirtualAlloc(%d): %s\n", Type, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
win_munmap(PVOID Address, SIZE_T Size)
{
	BOOL bResult = FALSE;
	MEMORY_BASIC_INFORMATION mbInfo = {0};

	if (!VirtualQuery(Address, &mbInfo, sizeof(MEMORY_BASIC_INFORMATION))){
		WIN_ERR("VirtualQuery(0x%x): %s\n", Address, win_strerror(GetLastError()));
	}else if (Size < mbInfo.RegionSize){
		bResult = VirtualFree(Address, 0, MEM_RELEASE);
	}else if (!VirtualAlloc(Address, Size, MEM_RESET, mbInfo.AllocationProtect)){
		WIN_ERR("VirtualAlloc(0x%x): %s\n", Address, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
//__PRINTF("Size(%d) Address(0x%x) State(0x%x) RegionSize(%d), Type(0x%x) BaseAddress(0x%x) AllocationBase(0x%x)\n", 
//		Size, Address, mbInfo.State, mbInfo.RegionSize, mbInfo.Type, mbInfo.BaseAddress, mbInfo.AllocationBase)
	return(bResult);
}
