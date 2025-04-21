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

DWORD 
win_SECURITY_DESCRIPTOR(PSECURITY_DESCRIPTOR Security, DWORD Type, LPSTR Buffer)
{
	LPSTR psz = Buffer;
	BOOL bPresent, bDefaulted;
	PSID pSid;
	PACL pAcl = NULL;
	DWORD dwRevision;
	SECURITY_DESCRIPTOR_CONTROL sdcFlags;

	if (GetSecurityDescriptorOwner(Security, &pSid, &bDefaulted)){
		psz = win_SID(psz, "Owner", pSid);
	}
	if (GetSecurityDescriptorGroup(Security, &pSid, &bDefaulted)){
		psz = win_SID(psz, "Group", pSid);
	}
	if (!GetSecurityDescriptorControl(Security, &sdcFlags, &dwRevision)){
		WIN_ERR("GetSecurityDescriptorControl(0x%x): %s\n", Security, win_strerror(GetLastError()));
	}else{
		psz += sprintf(psz, "Revision: 0x%x\n", dwRevision);
		psz = WinSecurityFlags(psz, "Flags", sdcFlags);
	}
	if (!GetSecurityDescriptorDacl(Security, &bPresent, &pAcl, &bDefaulted)){
		WIN_ERR("GetSecurityDescriptorDacl(0x%x): %s\n", Security, win_strerror(GetLastError()));
	}else{
		psz += sprintf(psz, "Present: %d\n", bPresent);
		psz += sprintf(psz, "Defaulted: %d\n", bDefaulted);
		if (bPresent && pAcl){
			psz = win_ACL(psz, "Discretionary ACL", pAcl, Type);
		}
	}
	return(psz - Buffer);
}
