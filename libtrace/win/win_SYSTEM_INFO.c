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

#define PROCESSOR_ARCHITECTURE_ARM64	12

#define POCESSOR_MODEL(a)	(a >> 8)
#define POCESSOR_STEPPING(a)	(a & 0xFF)

LPSTR 
win_SYSTEM_INFO(LPSTR Buffer, LPCSTR Label, SYSTEM_INFO *Info)
{
	LPSTR psz = Buffer;
	LPSTR pszArch = "UNKNOWN";

	switch (Info->wProcessorArchitecture){
		case PROCESSOR_ARCHITECTURE_AMD64:
			pszArch = "AMD64";
			break;
		case PROCESSOR_ARCHITECTURE_ARM:
			pszArch = "ARM";
			break;
		case PROCESSOR_ARCHITECTURE_ARM64:
			pszArch = "ARM64";
			break;
		case PROCESSOR_ARCHITECTURE_IA64:
			pszArch = "IA64";
			break;
		case PROCESSOR_ARCHITECTURE_INTEL:
			pszArch = "x86";
			break;
	}
	psz += sprintf(psz, "%s(%s): CPUType(%d) CPUMask(0x%x) CPUCount(%d) Level(%d) Model(%d) Stepping(%d)\n", 
		Label, pszArch, Info->dwProcessorType, Info->dwActiveProcessorMask, Info->dwNumberOfProcessors, 
		Info->wProcessorLevel, POCESSOR_MODEL(Info->wProcessorRevision), POCESSOR_STEPPING(Info->wProcessorRevision));
	psz += sprintf(psz, "  Addressable: 0x%x - 0x%x\n", Info->lpMinimumApplicationAddress, Info->lpMaximumApplicationAddress);
	psz += sprintf(psz, "  Ganularity: 0x%x\n", Info->dwAllocationGranularity);
	psz += sprintf(psz, "  PageSize: 0x%x\n", Info->dwPageSize);
	return(psz);
}
