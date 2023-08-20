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

#if __i386__
typedef DWORD TLSINT;
#else
typedef ULONGLONG TLSINT;
#endif

#define SELECTOR(index)		(index << 3) + 4 + 3

#define DT_LDT	2	// 00010
#define DT_DATA	18	// 10010
#define DT_CODE	26	// 11010

#define DPL_USER	3

typedef struct _PROCESS_LDT_INFORMATION {
	ULONG Start;
	ULONG Length;
	LDT_ENTRY LdtEntries[1];
} PROCESS_LDT_INFORMATION, *PPROCESS_LDT_INFORMATION;

/************************************************************/

VOID 
LdtDebug(DWORD Selector, LDT_ENTRY *Entry, LPCSTR Label)
{
	DWORD dwBase = (Entry->HighWord.Bits.BaseHi * 0x1000000) + Entry->BaseLow;
	DWORD dwLimit = (Entry->HighWord.Bits.LimitHi * 0x10000) + Entry->LimitLow;
	DWORD dwType = Entry->HighWord.Bits.Type & 0xF;
	DWORD dwDpl = Entry->HighWord.Bits.Dpl;
	BOOL bDescType = Entry->HighWord.Bits.Type >> 4;
	BOOL bGranular = Entry->HighWord.Bits.Granularity;
	BOOL bPresent = Entry->HighWord.Bits.Pres;
	BOOL bDefaultBig = Entry->HighWord.Bits.Default_Big;
	BOOL bSystem = Entry->HighWord.Bits.Sys;
	DWORD dwIndex = Selector >> 3;
	LPSTR pszTable = "gdt";

	dwBase += Entry->HighWord.Bits.BaseMid * 0x10000;
	if (Selector & 0x4){
		pszTable = "ldt";
	}
	__PRINTF("  %s(%s:%d):", Label, pszTable, dwIndex)
	__PRINTF(" select(0x%x)", Selector)
	__PRINTF(" type(%d:%d)", bDescType, dwType)
	__PRINTF(" base(0x%x)", dwBase)
	__PRINTF(" limit(0x%x)", dwLimit)
	__PRINTF(" dpl(%d)", dwDpl)
	__PRINTF(" big(%d)", bDefaultBig)
	__PRINTF(" gran(%d)", bGranular)
	__PRINTF(" pres(%d)", bPresent)
	__PRINTF(" sys(%d)", bSystem)
	__PRINTF("\n")
}
/* VOID 
LdtTest(HANDLE Handle, DWORD Selector, LPCSTR Label)
{
	LDT_ENTRY ldtEntry;

	if (!GetThreadSelectorEntry(Handle, Selector, &ldtEntry)){
		WIN_ERR("GetThreadSelectorEntry(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else{
		LdtDebug(Selector, &ldtEntry, Label);
	}
} */
VOID 
LdtSetEntry(DWORD Base, DWORD Limit, LDT_ENTRY *Result)
{
	DWORD dwBaseHi = Base >> 16;
	DWORD dwLimitHi = Limit >> 16;

	Result->BaseLow = Base & 0xFFFF;
	Result->LimitLow = Limit & 0xFFFF;
	Result->HighWord.Bits.BaseMid = dwBaseHi & 0xFF;
	Result->HighWord.Bits.BaseHi = dwBaseHi >> 8;
	Result->HighWord.Bits.Type = DT_DATA;
	Result->HighWord.Bits.Dpl = DPL_USER;
	Result->HighWord.Bits.Pres = 1;
	Result->HighWord.Bits.LimitHi = dwLimitHi >> 8;
	Result->HighWord.Bits.Granularity = 0;	// page (1) or byte (0)
	Result->HighWord.Bits.Sys = 0;
	Result->HighWord.Bits.Reserved_0 = 0;
	Result->HighWord.Bits.Default_Big = 1;	// > 64Kb
}

/************************************************************/

VOID 
win_ldt_attach(DWORD Count)
{
	ULONG ulLength = sizeof(LDT_ENTRY) * Count;
	ULONG ulSize = ulLength + sizeof(PROCESS_LDT_INFORMATION);
	PROCESS_LDT_INFORMATION *pldtInfo = win_malloc(ulSize);
	NTSTATUS ntStatus;

	pldtInfo->Start = 0;
	pldtInfo->Length = ulLength;
	/* Done with asm lldt/lgdt instructions on i386 */
	ntStatus = NtSetInformationProcess(GetCurrentProcess(), ProcessLdtInformation, pldtInfo, ulSize);
	if (!NT_SUCCESS(ntStatus)){
		WIN_ERR("NtSetInformationProcess(ProcessLdtInformation): %s\n", nt_strerror(ntStatus));
	}
	win_free(pldtInfo);
}
BOOL 
win_ldt_read(HANDLE Handle, DWORD Selector, LPCSTR Label)
{
	LDT_ENTRY ldtEntry;

	if (!GetThreadSelectorEntry(Handle, Selector, &ldtEntry)){
		WIN_ERR("GetThreadSelectorEntry(%d): %s\n", Handle, win_strerror(GetLastError()));
	}else{
		LdtDebug(Selector, &ldtEntry, Label);
	}
}
BOOL 
win_ldt_write(DWORD Selector, DWORD Base, DWORD Limit)
{
	DWORD dwIndex = Selector >> 3;
	ULONG ulLength = sizeof(LDT_ENTRY);
	ULONG ulStart = ulLength * dwIndex;
	ULONG ulSize = sizeof(PROCESS_LDT_INFORMATION);
	PROCESS_LDT_INFORMATION ldtInfo = {ulStart, ulLength, 0};
	NTSTATUS ntStatus;

	LdtSetEntry(Base, Limit, ldtInfo.LdtEntries);
	ntStatus = NtSetInformationProcess(GetCurrentProcess(), ProcessLdtInformation, &ldtInfo, ulSize);
	if (!NT_SUCCESS(ntStatus)){
		WIN_ERR("NtSetInformationProcess(ProcessLdtInformation): %s\n", nt_strerror(ntStatus));
	}
}
SHORT 
win_ldt_set(LPBYTE Data[], DWORD Index)
{
//	DWORD dwIndex = *(TLSINT *)Image->AddressOfIndex;
//	DWORD dwOffset = Image->StartAddressOfRawData;
	DWORD dwBase = __Offsets[0].Address - Data[0];
	DWORD dwLimit = __Offsets[0].Size;

	ULONG ulLength = sizeof(LDT_ENTRY);
	ULONG ulStart = ulLength * Index;
	ULONG ulSize = sizeof(PROCESS_LDT_INFORMATION);
	PROCESS_LDT_INFORMATION ldtInfo;
	NTSTATUS ntStatus;

__PRINTF("  Data: 0x%x\n", Data[0]);
	ldtInfo.Start = ulStart;
	ldtInfo.Length = ulLength;
	LdtSetEntry(dwBase, dwLimit, ldtInfo.LdtEntries);
	LdtDebug(SELECTOR(Index), ldtInfo.LdtEntries, "win_ldt_set");
	ntStatus = NtSetInformationProcess(GetCurrentProcess(), ProcessLdtInformation, &ldtInfo, ulSize);
	if (!NT_SUCCESS(ntStatus)){
		WIN_ERR("NtSetInformationProcess(ProcessLdtInformation): %s\n", nt_strerror(ntStatus));
	}
	return(SELECTOR(Index));
}
