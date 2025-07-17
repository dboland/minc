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

const CHAR *__STYPE[] = {
	"UNKNOWN",
	"user",
	"group",
	"domain",
	"alias",
	"well-known",
	"deleted-account",
	"invalid",
	"unknown",
	"computer",
	"label",
	"logon-session",
	""
};

LPSTR 
win_SID(LPSTR Buffer, LPCSTR Label, PSID Sid)
{
	LPSTR psz = Buffer;
	CHAR Account[UNLEN] = "UNKNOWN";
	CHAR Domain[UNLEN] = "UNKNOWN";
	DWORD accSize = UNLEN;
	DWORD domSize = UNLEN;
	SID_NAME_USE snuType = 0;

	LookupAccountSid(NULL, Sid, Account, &accSize, Domain, &domSize, &snuType);
	psz += sprintf(psz, "%s: %s (%s: %s\\%s)\n", Label, win_strsid(Sid), __STYPE[snuType], Domain, Account);
	return(psz);
}
