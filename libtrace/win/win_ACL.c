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

static const CHAR *__ATYPE[] = {
	"ACCESS_ALLOWED_ACE_TYPE",
	"ACCESS_DENIED_ACE_TYPE",
	"SYSTEM_AUDIT_ACE_TYPE",
	"SYSTEM_ALARM_ACE_TYPE",
	"ACCESS_ALLOWED_COMPOUND_ACE_TYPE",
	"ACCESS_ALLOWED_OBJECT_ACE_TYPE",
	"ACCESS_DENIED_OBJECT_ACE_TYPE",
	"SYSTEM_AUDIT_OBJECT_ACE_TYPE",
	"SYSTEM_ALARM_OBJECT_ACE_TYPE",
	"ACCESS_ALLOWED_CALLBACK_ACE_TYPE",
	"ACCESS_DENIED_CALLBACK_ACE_TYPE",
	"ACCESS_ALLOWED_CALLBACK_OBJECT_ACE_TYPE",
	"ACCESS_DENIED_CALLBACK_OBJECT_ACE_TYPE",
	"SYSTEM_AUDIT_CALLBACK_ACE_TYPE",
	"SYSTEM_ALARM_CALLBACK_ACE_TYPE",
	"SYSTEM_AUDIT_CALLBACK_OBJECT_ACE_TYPE",
	"SYSTEM_ALARM_CALLBACK_OBJECT_ACE_TYPE",
	"SYSTEM_MANDATORY_LABEL_ACE_TYPE",
	"SYSTEM_RESOURCE_ATTRIBUTE_ACE_TYPE",
	"SYSTEM_SCOPED_POLICY_ID_ACE_TYPE",
	""
};

LPSTR 
win_ACL(LPSTR Buffer, LPCSTR Label, PACL Acl, DWORD Type)
{
	LPSTR psz = Buffer;
	ACE_PEEK *Info;
	WORD wIndex = 0;

	psz += sprintf(psz, "%s(%d):\n", Label, Acl->AceCount);
	while (wIndex < Acl->AceCount){
		GetAce(Acl, wIndex, (PVOID)&Info);
		psz += sprintf(psz, "* ACE %d:\n", wIndex);
		psz = win_SID(psz, "  Trustee", &Info->Sid);
		psz = WinAceFlags(psz, "  AceFlags", Info->Header.AceFlags);
		psz = WinFileAccess(psz, Info->Mask, Type);
		psz += sprintf(psz, "  AceType(%d): %s\n", Info->Header.AceType, __ATYPE[Info->Header.AceType]);
		psz += sprintf(psz, "  AceSize: %d\n", Info->Header.AceSize);
		wIndex++;
	}
	return(psz);
}
