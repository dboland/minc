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

#include "config.h"

LPSTR 
WinCapabilities(LPSTR Buffer, LPCSTR Label, LSA_HANDLE Handle, PSID Sid)
{
	LPSTR psz = Buffer;
	PLSA_UNICODE_STRING pList = NULL;
	ULONG ulCount = 0;
	NTSTATUS ntStatus;

	ntStatus = LsaEnumerateAccountRights(Handle, Sid, &pList, &ulCount);
	psz += sprintf(psz, "%s(%d):\n", Label, ulCount);
	if (NT_SUCCESS(ntStatus)){
		while (ulCount--){
			if (wcsstr(pList->Buffer, L"Right")){
				psz += sprintf(psz, "  Right: %ls\n", pList->Buffer);
			}else{
				psz += sprintf(psz, "  Cap: %ls\n", pList->Buffer);
			}
			pList++;
		}
		LsaFreeMemory(pList);
	}else if (ntStatus != STATUS_OBJECT_NAME_NOT_FOUND){
		WIN_ERR("LsaEnumerateAccountRights(%s): %s\n", win_strsid(Sid), nt_strerror(ntStatus));
	}
	return(psz);
}
LPSTR 
win_SID_RIGHTS(LPSTR Buffer, LPCSTR Label, PSID Sid)
{
	LPSTR psz = Buffer;
	NTSTATUS ntStatus;
	LSA_OBJECT_ATTRIBUTES loa = {sizeof(loa), NULL, NULL, 0, NULL, NULL};
	LSA_HANDLE lsaHandle = NULL;

	ntStatus = LsaOpenPolicy(NULL, &loa, POLICY_LOOKUP_NAMES, &lsaHandle);
	if (!NT_SUCCESS(ntStatus)){
		WIN_ERR("LsaOpenPolicy(): %s\n", nt_strerror(ntStatus));
	}else{
		psz = WinCapabilities(psz, Label, lsaHandle, Sid);
		LsaClose(lsaHandle);
	}
	return(psz);
}
