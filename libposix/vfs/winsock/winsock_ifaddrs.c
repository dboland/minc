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
WSAGetIfFlags(PIP_ADAPTER_ADDRESSES Adapter)
{
	DWORD dwResult = WS_IFF_UP;

	if (Adapter->OperStatus == IfOperStatusUp){
		dwResult |= WS_IFF_RUNNING;
	}
	if (!(Adapter->Flags & IP_ADAPTER_NO_MULTICAST)){
		dwResult |= WS_IFF_MULTICAST;
	}
	switch (Adapter->IfType){
		case IF_TYPE_PPP:
			dwResult |= WS_IFF_POINTOPOINT;
			break;
		case IF_TYPE_SOFTWARE_LOOPBACK:
			dwResult |= WS_IFF_LOOPBACK;
			break;
		case IF_TYPE_ETHERNET_CSMACD:
		case IF_TYPE_IEEE80211:
			dwResult |= WS_IFF_BROADCAST;
			break;
	}
	return(dwResult);
}

/****************************************************/

BOOL 
ws2_setifaddrs(ULONG Family, WIN_IFENUM *Result)
{
	BOOL bResult = FALSE;
	ULONG ulStatus;
	PIP_ADAPTER_ADDRESSES pTable;
	LONG lSize = 0;
	ULONG ulFlags = GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_MULTICAST;

	ulStatus = GetAdaptersAddresses(Family, ulFlags, NULL, NULL, &lSize);
	if (lSize > 0){
		pTable = win_malloc(lSize);
		GetAdaptersAddresses(Family, ulFlags, NULL, pTable, &lSize);
		Result->Table = pTable;
		Result->Next = pTable;
		bResult = TRUE;
//	}else{
//		WIN_ERR("GetAdaptersAddresses(%d): %s", Family, win_strerror(ulStatus));
	}
	return(bResult);
}
VOID 
ws2_endifaddrs(WIN_IFENUM *Enum)
{
	win_free(Enum->Table);
}
BOOL 
ws2_getifaddrs(WIN_IFENUM *Enum, WIN_IFENT *Result)
{
	BOOL bResult = FALSE;
	PIP_ADAPTER_ADDRESSES pAdapter = Enum->Next;

	if (!pAdapter){
		SetLastError(ERROR_NO_MORE_ITEMS);
	}else{
		Result->IfIndex = pAdapter->IfIndex;
		Result->IfType = pAdapter->IfType;
		Result->IfFlags = WSAGetIfFlags(pAdapter);
		Result->Mtu = pAdapter->Mtu;
		Result->Unicast = pAdapter->FirstUnicastAddress;
		Result->AddrLen = pAdapter->PhysicalAddressLength;
		win_memcpy(Result->PhysAddr, pAdapter->PhysicalAddress, Result->AddrLen);
		Enum->Next = pAdapter->Next;
		bResult = TRUE;
	}
	return(bResult);
}

