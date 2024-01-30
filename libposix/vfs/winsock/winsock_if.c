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

#include <iphlpapi.h>

/****************************************************/

DWORD 
WSAGetIfFlags(PIP_ADAPTER_ADDRESSES Adapter)
{
	DWORD dwResult = WIN_IFF_UP;

	if (Adapter->OperStatus == IfOperStatusUp){
		dwResult |= WIN_IFF_RUNNING;
	}
//	if (Mask){
//		result |= WIN_IFF_BROADCAST;
//	}
	if (!(Adapter->Flags & IP_ADAPTER_NO_MULTICAST)){
		dwResult |= WIN_IFF_MULTICAST;
	}
	switch (Adapter->IfType){
		case IF_TYPE_PPP:
			dwResult |= WIN_IFF_POINTOPOINT;
			break;
		case IF_TYPE_SOFTWARE_LOOPBACK:
			dwResult |= WIN_IFF_LOOPBACK;
			break;
	}
	return(dwResult);
}

/****************************************************/

BOOL 
ws2_setifent(WIN_IFENUM *Enum)
{
	BOOL bResult = FALSE;
	ULONG ulStatus;
	PIP_ADAPTER_ADDRESSES pTable;
	LONG lSize = 0;
	ULONG ulFlags = GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_MULTICAST;

	ulStatus = GetAdaptersAddresses(AF_UNSPEC, ulFlags, NULL, NULL, &lSize);
	if (lSize > 0){
		pTable = win_malloc(lSize);
		GetAdaptersAddresses(AF_UNSPEC, ulFlags, NULL, pTable, &lSize);
		Enum->Table = pTable;
		Enum->Next = pTable;
		bResult = TRUE;
	}else{
		WIN_ERR("GetAdaptersAddresses(AF_UNSPEC): %s", win_strerror(ulStatus));
	}
	return(bResult);
}
VOID 
ws2_endifent(WIN_IFENUM *Enum)
{
	win_free(Enum->Table);
}
BOOL 
ws2_getifent(WIN_IFENUM *Enum, WIN_IFENT *Result)
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
		win_wcstombs(Result->Description, pAdapter->Description, MAXLEN_IFDESCR);
		Enum->Next = pAdapter->Next;
		bResult = TRUE;
	}
	return(bResult);
}

/****************************************************/

UINT 
ws2_nametoindex(LPCSTR Name)
{
	WIN_DEVICE *pwDevice = DEVICE(DEV_CLASS_IFNET);
	USHORT sUnit = 0;
	UINT uiError = ERROR_BAD_DEVICE;

	while (sUnit < WIN_UNIT_MAX){
		if (!win_strcmp(pwDevice->Name, Name)){
			if (!pwDevice->Flags){
				uiError = ERROR_DEVICE_NOT_AVAILABLE;
				break;
			}else{
				return(pwDevice->Index);
			}
		}
		pwDevice++;
		sUnit++;
	}
	SetLastError(uiError);
	return(0);
}
UINT 
ws2_indextoname(DWORD Index, LPSTR Result)
{
	WIN_DEVICE *pwDevice = DEVICE(DEV_CLASS_IFNET);
	USHORT sUnit = 0;
	UINT uiError = ERROR_BAD_DEVICE;

	if (!Index){		/* XP */
		uiError = ERROR_INVALID_PARAMETER;
	}else while (sUnit < WIN_UNIT_MAX){
		if (pwDevice->Index == Index){
			win_strcpy(Result, pwDevice->Name);
			return(win_strlen(Result));
		}
		pwDevice++;
		sUnit++;
	}
	SetLastError(uiError);
	return(0);
}
