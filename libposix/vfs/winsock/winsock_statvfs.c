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
WSALookup(DWORD LinkType)
{
	DWORD dwResult = DEV_CLASS_IFNET;

	switch (LinkType){
		case IF_TYPE_ETHERNET_CSMACD:
			dwResult = DEV_TYPE_ETH;
			break;
		case IF_TYPE_PPP:
			dwResult = DEV_TYPE_PPP;
			break;
		case IF_TYPE_SOFTWARE_LOOPBACK:
			dwResult = DEV_TYPE_LOOPBACK;
			break;
		case IF_TYPE_IEEE80211:
			dwResult = DEV_TYPE_WLAN;
			break;
		case IF_TYPE_TUNNEL:
			dwResult = DEV_TYPE_TUNNEL;
			break;
	}
	return(dwResult);
}
LPWSTR 
WSALinkName(LPWSTR Buffer, UCHAR Source[], LONG Length)
{
	WCHAR *psz = Buffer;

	*Buffer = 0;
	if (Length){
		psz += msvc_swprintf(psz, L"%.2X", *Source++);
		Length--;
	}
	while (Length){
		psz += msvc_swprintf(psz, L"-%.2X", *Source++);
		Length--;
	}
	return(Buffer);
}

/****************************************************/

BOOL 
ws2_setvfs(WIN_IFDATA *Config, BOOL Ascending, PMIB_IFROW *Interface, DWORD *Count)
{
	BOOL bResult = FALSE;
	PMIB_IFTABLE pifTable = NULL;
	PMIB_IFROW pifRow;
	DWORD dwCount;

	if (ws2_NET_RT_IFLIST(&pifTable, &pifRow, &dwCount)){
		Config->Table = pifTable;
		if (Ascending){
			*Interface = pifTable->table;
		}else{
			*Interface = pifRow;
		}
		*Count = dwCount;
		bResult = TRUE;
	}
	return(bResult);
}
VOID 
ws2_endvfs(WIN_IFDATA *Config)
{
	win_free(Config->Table);
}
BOOL 
ws2_statvfs(WIN_IFDATA *Config, PMIB_IFROW Interface)
{
	BOOL bResult = TRUE;

	Config->DeviceType = WSALookup(Interface->dwType);
	Config->FSType = FS_TYPE_WINSOCK;
	if (Interface->dwPhysAddrLen){
		WSALinkName(Config->NtName, Interface->bPhysAddr, Interface->dwPhysAddrLen);
	}else if (Config->DeviceType == DEV_TYPE_LOOPBACK){
		win_wcscpy(Config->NtName, L"00-00-00-00-00-00");
	}else{
		/* Vista */
		Config->FSType = FS_TYPE_NDIS;
		win_wcscpy(Config->NtName, win_basename(Interface->wszName));
		bResult = FALSE;
	}
	return(bResult);
}
