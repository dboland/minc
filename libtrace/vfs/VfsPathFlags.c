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
VfsPathFlags(LPSTR Buffer, LPCSTR Label, DWORD Flags)
{
	LPSTR psz = Buffer;

	psz += sprintf(psz, "%s([0x%x]", Label, Flags);
	psz = WinFlagName(psz, WIN_PATHCOPY, "PATHCOPY", Flags, &Flags);
//	psz = WinFlagName(psz, WIN_REQUIREDEVICE, "REQUIREDEVICE", Flags, &Flags);
	psz = WinFlagName(psz, WIN_REQUIREOBJECT, "REQUIREOBJECT", Flags, &Flags);
	psz = WinFlagName(psz, WIN_LOCKLEAF, "LOCKLEAF", Flags, &Flags);
	psz = WinFlagName(psz, WIN_FOLLOW, "FOLLOW", Flags, &Flags);
	psz = WinFlagName(psz, WIN_NOCROSSMOUNT, "NOCROSSMOUNT", Flags, &Flags);
	psz = WinFlagName(psz, WIN_RDONLY, "ISSYMLINK", Flags, &Flags);
	psz = WinFlagName(psz, WIN_HASBUF, "REQUIREDIR", Flags, &Flags);
	psz = WinFlagName(psz, WIN_SAVENAME, "STRIPSLASHES", Flags, &Flags);
	psz = WinFlagName(psz, WIN_SAVESTART, "SAVESTART", Flags, &Flags);
	psz = WinFlagName(psz, WIN_ISDOTDOT, "ISDOTDOT", Flags, &Flags);
	psz = WinFlagName(psz, WIN_MAKEENTRY, "MAKEENTRY", Flags, &Flags);
	psz = WinFlagName(psz, WIN_ISLASTCN, "ISLASTCN", Flags, &Flags);
	psz = WinFlagName(psz, WIN_ISSYMLINK, "ISSYMLINK", Flags, &Flags);
	psz = WinFlagName(psz, WIN_REQUIREDIR, "REQUIREDIR", Flags, &Flags);
	psz = WinFlagName(psz, WIN_STRIPSLASHES, "STRIPSLASHES", Flags, &Flags);
	psz = WinFlagName(psz, WIN_PDIRUNLOCK, "PDIRUNLOCK", Flags, &Flags);
	psz += sprintf(psz, "[0x%x])\n", Flags);
	return(psz);
}
