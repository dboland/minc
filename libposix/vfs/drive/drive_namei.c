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

#include <winbase.h>

/****************************************************/

BOOL 
drive_lookup(WIN_NAMEIDATA *Path, DWORD Flags)
{
	LONG lMountId = MOUNTID(Path->Base[0]);
	WIN_MOUNT *pwMount = &__Mounts[lMountId];

//vfs_ktrace("vol_lookup", STRUCT_NAMEI, Path);
	if (lMountId < 0 || lMountId >= WIN_MOUNT_MAX){
		SetLastError(ERROR_BAD_ARGUMENTS);
	}else if (Flags & WIN_NOCROSSMOUNT){	/* vfs_unmount() */
		Path->MountId = lMountId;
	}else if (!pwMount->DeviceId){
		SetLastError(ERROR_DEVICE_NOT_AVAILABLE);
	}else{
		Path->MountId = pwMount->MountId;
		if (Flags & WIN_REQUIREDIR){
			Path->R = win_wcpcpy(Path->Resolved, pwMount->Path);
		}else{
			Path->R = win_wcpcpy(Path->Resolved, pwMount->Drive);
		}
		Path->Base = Path->R;
	}
	return(TRUE);
}
