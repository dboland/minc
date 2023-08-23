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

#include <iprtrmib.h>

/****************************************************/

DWORD 
GetAddrEntry(MIB_IPADDRROW *Address)
{
	DWORD dwStatus;
	MIB_IPADDRTABLE *pipaTable;
	MIB_IPADDRROW *pipaRow;
	LONG lSize = 0;
	DWORD dwCount = 0;
	DWORD dwIndex = Address->dwIndex;

	dwStatus = GetIpAddrTable(NULL, &lSize, FALSE);
	if (lSize > 0){
		pipaTable = win_malloc(lSize);
		dwStatus = GetIpAddrTable(pipaTable, &lSize, FALSE);
		pipaRow = pipaTable->table;
		dwCount = pipaTable->dwNumEntries;
		while (dwCount--){
			if (pipaRow->dwIndex == dwIndex && pipaRow->wType & MIB_IPADDR_PRIMARY){
				win_memcpy(Address, pipaRow, sizeof(MIB_IPADDRROW));
				break;
			}
			pipaRow++;
		}
		win_free(pipaTable);
	}else{
		WIN_ERR("GetIpAddrTable(%d): %s\n", dwIndex, win_strerror(dwStatus));
	}
	return(dwStatus);
}
ULONG 
GetAdapterEntry(PIP_ADAPTER_ADDRESSES *Table, DWORD Index, PIP_ADAPTER_ADDRESSES *Result)
{
	ULONG ulStatus;
	PIP_ADAPTER_ADDRESSES pifaRow;
	LONG lSize = 0;
	ULONG ulFlags = GAA_FLAG_INCLUDE_PREFIX;

	ulStatus = GetAdaptersAddresses(AF_UNSPEC, ulFlags, NULL, NULL, &lSize);
	if (lSize > 0){
		pifaRow = win_malloc(lSize);
		ulStatus = GetAdaptersAddresses(AF_UNSPEC, ulFlags, NULL, pifaRow, &lSize);
		*Table = pifaRow;
		while (pifaRow){
			if (pifaRow->IfIndex == Index){
				*Result = pifaRow;
				break;
			}
			pifaRow = pifaRow->Next;
		}
	}else{
		WIN_ERR("GetAdaptersAddresses(): %s\n", win_strerror(ulStatus));
	}
	return(ulStatus);
}
ULONG 
GetAdapterAddress(PIP_ADAPTER_ADDRESSES *Table, DWORD Index, WS2_ADDRTYPE Type, PVOID *Result)
{
	ULONG ulStatus;
	PIP_ADAPTER_ADDRESSES pifaRow;

	ulStatus = GetAdapterEntry(Table, Index, &pifaRow);
	if (ulStatus != ERROR_SUCCESS){
		return(ulStatus);
	}else switch (Type){
		case WS2_UNICAST:
			*(PIP_ADAPTER_UNICAST_ADDRESS *)Result = pifaRow->FirstUnicastAddress;
			break;
		case WS2_ANYCAST:
			*(PIP_ADAPTER_ANYCAST_ADDRESS *)Result = pifaRow->FirstAnycastAddress;
			break;
		case WS2_MULTICAST:
			*(PIP_ADAPTER_MULTICAST_ADDRESS *)Result = pifaRow->FirstMulticastAddress;
			break;
		case WS2_DNSSERVER:
			*(PIP_ADAPTER_DNS_SERVER_ADDRESS *)Result = pifaRow->FirstDnsServerAddress;
			break;
	}
	return(ulStatus);
}
