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

BOOL 
ws2_NET_RT_DUMP(PMIB_IPFORWARDTABLE *Table, PMIB_IPFORWARDROW *Row, DWORD *Count)
{
	BOOL bResult = FALSE;
	PMIB_IPFORWARDTABLE pfwTable;
	ULONG ulSize = 0;
	DWORD dwStatus;

	dwStatus = GetIpForwardTable(NULL, &ulSize, FALSE);
	if (!ulSize){
		WIN_ERR("GetIpForwardTable(): %s\n", win_strerror(dwStatus));
	}else{
		pfwTable = win_malloc(ulSize);
		GetIpForwardTable(pfwTable, &ulSize, FALSE);
		*Table = pfwTable;
		*Row = pfwTable->table;
		*Count = pfwTable->dwNumEntries;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
ws2_NET_RT_IFLIST(PMIB_IFTABLE *Table, PMIB_IFROW *Row, DWORD *Count)
{
	BOOL bResult = FALSE;
	ULONG ulSize = 0;
	DWORD dwStatus;
	MIB_IFTABLE *ifTable;

	dwStatus = GetIfTable(NULL, &ulSize, FALSE);
	if (!ulSize){
		WIN_ERR("GetIfTable(): %s", win_strerror(dwStatus));
	}else{
		ifTable = win_malloc(ulSize);
		GetIfTable(ifTable, &ulSize, FALSE);
		*Table = ifTable;
		*Row = &ifTable->table[ifTable->dwNumEntries - 1];
		*Count = ifTable->dwNumEntries;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
ws2_NET_RT_IFALIST(PMIB_IPADDRTABLE *Table, PMIB_IPADDRROW *Row, DWORD *Count)
{
	BOOL bResult = FALSE;
	ULONG ulSize = 0;
	DWORD dwStatus;
	MIB_IPADDRTABLE *addrTable;

	dwStatus = GetIpAddrTable(NULL, &ulSize, FALSE);
	if (!ulSize){
		WIN_ERR("GetIpAddrTable(): %s", win_strerror(dwStatus));
	}else{
		addrTable = win_malloc(ulSize + sizeof(MIB_IPADDRROW));
		GetIpAddrTable(addrTable, &ulSize, FALSE);
		*Table = addrTable;
		*Row = &addrTable->table[addrTable->dwNumEntries - 1];
		*Count = addrTable->dwNumEntries;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
ws2_NET_RT_UNKNOWN(PMIB_TCPTABLE *Table, PMIB_TCPROW *Row, DWORD *Count)
{
	BOOL bResult = FALSE;
	PMIB_TCPTABLE ptcpTable;
	ULONG ulSize = 0;
	DWORD dwStatus;

	dwStatus = GetTcpTable(NULL, &ulSize, FALSE);
	if (!ulSize){
		WIN_ERR("GetTcpTable(): %s\n", win_strerror(dwStatus));
	}else{
		ptcpTable = win_malloc(ulSize);
		GetTcpTable(ptcpTable, &ulSize, FALSE);
		*Table = ptcpTable;
		*Row = ptcpTable->table;
		*Count = ptcpTable->dwNumEntries;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
ws2_NET_RT_OACTIVE(PMIB_IPNETTABLE *Table, PMIB_IPNETROW *Row, DWORD *Count)
{
	BOOL bResult = FALSE;
	PMIB_IPNETTABLE pinTable;
	ULONG ulSize = 0;
	DWORD dwStatus;

	dwStatus = GetIpNetTable(NULL, &ulSize, FALSE);
	if (!ulSize){
		WIN_ERR("GetIpNetTable(): %s\n", win_strerror(dwStatus));
	}else{
		pinTable = win_malloc(ulSize);
		GetIpNetTable(pinTable, &ulSize, FALSE);
		*Table = pinTable;
		*Row = pinTable->table;
		*Count = pinTable->dwNumEntries;
		bResult = TRUE;
	}
	return(bResult);
}
