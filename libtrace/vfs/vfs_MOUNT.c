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

DWORD 
vfs_MOUNT(WIN_MOUNT *Mount, LPSTR Buffer)
{
	LPSTR psz = Buffer;

	if (Mount->Serial){
		psz += sprintf(psz, "%ls(%ls) MountId(%d) MaxPath(%d) DevType(0x%x) DevId(0x%x)\n", 
			Mount->Label, Mount->TypeName, Mount->MountId, Mount->MaxPath, Mount->DeviceType, Mount->DeviceId);
		psz = VfsVolumeFlagsHigh(psz, "+ FlagsHigh", Mount->Flags.HighPart);
		psz = VfsVolumeFlagsLow(psz, "+ FlagsLow", Mount->Flags.LowPart);
		psz += sprintf(psz, "+ Serial: %lu\n", Mount->Serial);
		psz += sprintf(psz, "+ Volume: %ls\n", Mount->Volume);
		psz += sprintf(psz, "+ Path: %ls\n", Mount->Path);
	}else{
		SetLastError(ERROR_NOT_READY);
	}
	return(psz - Buffer);
}
