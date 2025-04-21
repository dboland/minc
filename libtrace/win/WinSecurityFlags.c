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
WinSecurityFlags(LPSTR Buffer, LPCSTR Label, WORD Flags)
{
	LPSTR psz = Buffer;
	DWORD dwRemain = Flags;
#define SE_RM_CONTROL_VALID	0x4000

	psz += sprintf(psz, "%s(0x%x): ", Label, Flags);
	psz = WinFlagName(psz, SE_DACL_AUTO_INHERIT_REQ, "DACL_AUTO_INHERIT_REQ", dwRemain, &dwRemain);
	psz = WinFlagName(psz, SE_DACL_AUTO_INHERITED, "DACL_AUTO_INHERITED", dwRemain, &dwRemain);
	psz = WinFlagName(psz, SE_DACL_DEFAULTED, "DACL_DEFAULTED", dwRemain, &dwRemain);
	psz = WinFlagName(psz, SE_DACL_PRESENT, "DACL_PRESENT", dwRemain, &dwRemain);
	psz = WinFlagName(psz, SE_DACL_PROTECTED, "DACL_PROTECTED", dwRemain, &dwRemain);
	psz = WinFlagName(psz, SE_GROUP_DEFAULTED, "GROUP_DEFAULTED", dwRemain, &dwRemain);
	psz = WinFlagName(psz, SE_OWNER_DEFAULTED, "OWNER_DEFAULTED", dwRemain, &dwRemain);
	psz = WinFlagName(psz, SE_RM_CONTROL_VALID, "CONTROL_VALID", dwRemain, &dwRemain);
	psz = WinFlagName(psz, SE_SACL_AUTO_INHERIT_REQ, "SACL_AUTO_INHERIT_REQ", dwRemain, &dwRemain);
	psz = WinFlagName(psz, SE_SACL_AUTO_INHERITED, "SACL_AUTO_INHERITED", dwRemain, &dwRemain);
	psz = WinFlagName(psz, SE_SACL_DEFAULTED, "SACL_DEFAULTED", dwRemain, &dwRemain);
	psz = WinFlagName(psz, SE_SACL_PRESENT, "SACL_PRESENT", dwRemain, &dwRemain);
	psz = WinFlagName(psz, SE_SACL_PROTECTED, "SACL_PROTECTED", dwRemain, &dwRemain);
	psz = WinFlagName(psz, SE_SELF_RELATIVE, "SELF_RELATIVE", dwRemain, &dwRemain);
	psz += sprintf(psz, "[0x%x]\n", dwRemain);
	return(psz);
}
