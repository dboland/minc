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
MManAlloc(PVOID Address, DWORD Size, DWORD Protect, PVOID *Result)
{
	BOOL bResult = FALSE;
	PVOID pvResult = NULL;

	if (pvResult = VirtualAlloc(Address, Size, MEM_COMMIT, Protect)){
		*Result = pvResult;
		bResult = TRUE;
	}else{
		WIN_ERR("VirtualAlloc(%d): %s\n", Size, win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
MManLoadFile(HANDLE Handle, PVOID Address, LONG Offset, DWORD Size, PVOID *Result)
{
	BOOL bResult = FALSE;
	LONG lBytesRead;

	if (INVALID_SET_FILE_POINTER == SetFilePointer(Handle, Offset, NULL, FILE_BEGIN)){
		WIN_ERR("SetFilePointer(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else if (!ReadFile(Handle, Address, Size, &lBytesRead, NULL)){
		WIN_ERR("ReadFile(%d): %s\n", Address, win_strerror(GetLastError()));
	}else if (INVALID_SET_FILE_POINTER == SetFilePointer(Handle, Offset, NULL, FILE_BEGIN)){
		WIN_ERR("SetFilePointer(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else{
		*Result = Address;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
MManCreateFile(HANDLE Handle, PVOID Address, LONG Offset, DWORD Size, PVOID *Result)
{
	BOOL bResult = FALSE;
	HANDLE hResult = NULL;
	LPVOID lpvResult = NULL;

	if (!(hResult = CreateFileMapping(Handle, NULL, PAGE_READWRITE, 0, 0, NULL))){
		WIN_ERR("CreateFileMapping(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else if (lpvResult = MapViewOfFile(hResult, FILE_MAP_WRITE, 0, 0, 0)){
		*Result = lpvResult;
		bResult = TRUE;
	}
	return(bResult);
}

/************************************************************/

BOOL 
win_mprotect(PVOID Address, DWORD Size, DWORD Protect)
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
win_mmap(HANDLE Handle, LONG Offset, DWORD Size, PVOID Address, DWORD Protect, PVOID *Result)
{
	BOOL bResult = FALSE;
	PVOID pvResult = NULL;

	if (Handle == INVALID_HANDLE_VALUE){
		bResult = MManAlloc(Address, Size, Protect, Result);
	}else if (Address){
		bResult = MManLoadFile(Handle, Address, Offset, Size, Result);
	}else if (!MManAlloc(NULL, Size, PAGE_READWRITE, &pvResult)){
		return(FALSE);
	}else if (!MManLoadFile(Handle, pvResult, Offset, Size, Result)){
		VirtualFree(pvResult, 0, MEM_DECOMMIT);
	}else{
		bResult = win_mprotect(pvResult, Size, Protect);
	}
	return(bResult);
}
BOOL 
win_munmap(PVOID Address, DWORD Offset)
{
	BOOL bResult = FALSE;

	if (!VirtualFree(Address, 0, MEM_RELEASE)){
		WIN_ERR("VirtualFree(0x%x): %s\n", Address, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
