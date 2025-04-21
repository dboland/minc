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
win_TOKEN_PRIVILEGES(LPSTR Buffer, LPSTR Label, PTOKEN_PRIVILEGES Privileges)
{
	LPSTR psz = Buffer;
	DWORD dwCount = Privileges->PrivilegeCount;
	LUID_AND_ATTRIBUTES *plEntry = Privileges->Privileges;
	CHAR szName[UNLEN];
	DWORD dwSize;

	psz += sprintf(psz, "%s(%d):\n", Label, dwCount);
	while (dwCount--){
		dwSize = UNLEN;
		if (!LookupPrivilegeName(NULL, &plEntry->Luid, szName, &dwSize)){
			WIN_ERR("LookupPrivilegeName(0x%x): %s\n", plEntry->Luid.LowPart, win_strerror(GetLastError()));
		}else{
			psz = WinPrivilegeAttribs(psz, szName, plEntry->Attributes);
		}
		plEntry++;
	}
	return(psz);
}
