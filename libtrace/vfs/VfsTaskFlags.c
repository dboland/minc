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
VfsTaskFlags(LPSTR Buffer, LPCSTR Label, DWORD Flags)
{
	LPSTR psz = Buffer;

	psz += sprintf(psz, "%s([0x%x]", Label, Flags);
	psz = WinFlagName(psz, WIN_PS_CONTROLT, "PS_CONTROLT", Flags, &Flags);
	psz = WinFlagName(psz, WIN_PS_EXEC, "PS_EXEC", Flags, &Flags);
	psz = WinFlagName(psz, WIN_PS_INEXEC, "PS_INEXEC", Flags, &Flags);
	psz = WinFlagName(psz, WIN_PS_EXITING, "PS_EXITING", Flags, &Flags);
	psz = WinFlagName(psz, WIN_PS_PPWAIT, "PS_PPWAIT", Flags, &Flags);
	psz = WinFlagName(psz, WIN_PS_ISPWAIT, "PS_ISPWAIT", Flags, &Flags);
	psz = WinFlagName(psz, WIN_PS_TRACED, "PS_TRACED", Flags, &Flags);
	psz = WinFlagName(psz, WIN_PS_NOZOMBIE, "PS_NOZOMBIE", Flags, &Flags);
	psz = WinFlagName(psz, WIN_PS_STOPPED, "PS_STOPPED", Flags, &Flags);
	psz = WinFlagName(psz, WIN_PS_SYSTEM, "PS_SYSTEM", Flags, &Flags);
	psz = WinFlagName(psz, WIN_PS_EMBRYO, "PS_EMBRYO", Flags, &Flags);
	psz = WinFlagName(psz, WIN_PS_ZOMBIE, "PS_ZOMBIE", Flags, &Flags);
	psz += sprintf(psz, "[0x%x])\n", Flags);
	return(psz);
}
