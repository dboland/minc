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

PIMAGE_NT_HEADERS NTAPI ImageNtHeader(PVOID);

/************************************************************/

BOOL 
win_getrlimit_DATA(WIN_RLIMIT *Limit)
{
	IMAGE_NT_HEADERS *intHeaders = ImageNtHeader(GetModuleHandle(NULL));
	WORD wCount = intHeaders->FileHeader.NumberOfSections;
	IMAGE_SECTION_HEADER *isHeader = (IMAGE_SECTION_HEADER *)(intHeaders + 1);
	HANDLE hHeap = GetProcessHeap();
	PROCESS_HEAP_ENTRY phEntry = {0};

	/* GetProcessWorkingSetSize()? */
	Limit->Current = 0;
	while (wCount--){
		if (!win_strncmp(isHeader->Name, ".data", 5)){
			Limit->Current += isHeader->Misc.VirtualSize;
		}else if (!win_strncmp(isHeader->Name, ".rdata", 6)){
			Limit->Current += isHeader->Misc.VirtualSize;
		}
		isHeader++;
	}
	Limit->Maximum = Limit->Current;
	while (HeapWalk(hHeap, &phEntry)){
		if (!phEntry.Region.dwUnCommittedSize){
			break;
		}
		Limit->Current += phEntry.Region.dwCommittedSize;
		Limit->Maximum += phEntry.Region.dwCommittedSize + phEntry.Region.dwUnCommittedSize;
	}
	return(TRUE);
}
BOOL 
win_getrlimit_AS(WIN_RLIMIT *Limit)
{
	MEMORY_BASIC_INFORMATION mbInfo;
	BOOL bResult = FALSE;

	if (!VirtualQuery(&mbInfo, &mbInfo, sizeof(MEMORY_BASIC_INFORMATION))){
		WIN_ERR("VirtualQuery(%s): %s\n", "RLIMIT_AS", win_strerror(GetLastError()));
	}else{
		Limit->Current = mbInfo.BaseAddress - mbInfo.AllocationBase;
		Limit->Maximum = mbInfo.BaseAddress + mbInfo.RegionSize - mbInfo.AllocationBase;
		bResult = TRUE;
	}
	return(bResult);
}
