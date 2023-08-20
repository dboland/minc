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

BOOL 
ws2_SIOGET_IPADDRROW(MIB_IPADDRROW *Address)
{
	MIB_IPADDRTABLE *paddrTable;
	MIB_IPADDRROW *paddrRow;
	LONG lSize = 0;
	BOOL bResult = FALSE;
	DWORD dwStatus;
	DWORD dwCount = 0;
	DWORD dwIndex = Address->dwIndex;

	dwStatus = GetIpAddrTable(NULL, &lSize, FALSE);
	if (lSize > 0){
		paddrTable = win_malloc(lSize);
		GetIpAddrTable(paddrTable, &lSize, FALSE);
		paddrRow = paddrTable->table;
		dwCount = paddrTable->dwNumEntries;
		while (dwCount--){
			if (paddrRow->dwIndex == dwIndex && paddrRow->wType & MIB_IPADDR_PRIMARY){
				win_memcpy(Address, paddrRow, sizeof(MIB_IPADDRROW));
				bResult = TRUE;
				break;
			}
			paddrRow++;
		}
		win_free(paddrTable);
	}else{
		WIN_ERR("GetIpAddrTable(%d): %s\n", dwIndex, win_strerror(dwStatus));
	}
	return(bResult);
}
