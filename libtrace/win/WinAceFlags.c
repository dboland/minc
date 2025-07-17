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

#include "../config.h"

LPSTR 
WinAceFlags(LPSTR Buffer, LPCSTR Label, BYTE Flags)
{
	LPSTR psz = Buffer;
	DWORD dwRemain = (DWORD)Flags;

	psz += sprintf(psz, "%s(0x%x): ", Label, Flags);
	psz = WinFlagName(psz, FAILED_ACCESS_ACE_FLAG, "FAILED_ACCESS", dwRemain, &dwRemain);
	psz = WinFlagName(psz, SUCCESSFUL_ACCESS_ACE_FLAG, "SUCCESSFUL_ACCESS", dwRemain, &dwRemain);
	psz = WinFlagName(psz, INHERITED_ACE, "INHERITED", dwRemain, &dwRemain);
	psz = WinFlagName(psz, INHERIT_ONLY_ACE, "INHERIT_ONLY", dwRemain, &dwRemain);
	psz = WinFlagName(psz, NO_PROPAGATE_INHERIT_ACE, "NO_PROPAGATE_INHERIT", dwRemain, &dwRemain);
	psz = WinFlagName(psz, CONTAINER_INHERIT_ACE, "CONTAINER_INHERIT", dwRemain, &dwRemain);
	psz = WinFlagName(psz, OBJECT_INHERIT_ACE, "OBJECT_INHERIT", dwRemain, &dwRemain);
	psz += sprintf(psz, "[0x%x]\n", dwRemain);
	return(psz);
}
