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
ptm_open(WIN_DEVICE *Device, WIN_FLAGS *Flags, WIN_VNODE *Result)
{
	BOOL bResult = FALSE;

	switch (Device->FSType){
		case FS_TYPE_MAILSLOT:
			bResult = mail_open(Device, Flags, Result);
			break;
		case FS_TYPE_CHAR:
			bResult = char_open(Device, Flags, Result);
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
//vfs_ktrace("ptm_open", STRUCT_DEVICE, Device);
	return(bResult);
}
BOOL 
ptm_revoke(WIN_DEVICE *Device)
{
	BOOL bResult = FALSE;

	if (!CloseHandle(Device->Input)){
		WIN_ERR("CloseHandle(%d): %s\n", Device->Input, win_strerror(GetLastError()));
	}else if (!CloseHandle(Device->Output)){
		WIN_ERR("CloseHandle(%d): %s\n", Device->Output, win_strerror(GetLastError()));
	}else if (!CloseHandle(Device->Handle)){
		WIN_ERR("CloseHandle(%d): %s\n", Device->Handle, win_strerror(GetLastError()));
	}else{
		ZeroMemory(Device, sizeof(WIN_DEVICE));
		bResult = TRUE;
	}
	return(bResult);
}