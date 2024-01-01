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

/* The Windows CHAR file system seemed quirky until I realized that it
 * is used to implement the WinNT pseudo terminal. This becomes more 
 * apparent in Vista, where a PTY was implemented in the MAILSLOT
 * file system.
 * For instance, opening CONIN$ and CONOUT$ is equivalent to opening /dev/tty.
 * But the resulting handles are not the system console (despite of the 
 * semantics), because if used outside of the current CMD Window, they
 * are invalid.
 * Trying to DuplicateHandle() parent's STD_XXX_HANDLE while in a child
 * process results in "The parameter is incorrect", but the Handle is
 * valid anyway. In Vista the error is "The handle is invalid".
 */

/****************************************************/

HANDLE 
CharOpenFile(LPCSTR Name, WIN_FLAGS *Flags, PSECURITY_ATTRIBUTES sa)
{
	HANDLE hResult = NULL;

	hResult = CreateFile(Name, Flags->Access, Flags->Share, sa, 
		Flags->Creation, Flags->Attribs, NULL);
	if (hResult == INVALID_HANDLE_VALUE){
		WIN_ERR("CreateFile(%s): %s\n", Name, win_strerror(GetLastError()));
	}
	return(hResult);
}
BOOL 
CharStatFile(WIN_DEVICE *Device, WIN_VATTR *Result)
{
	BOOL bResult = FALSE;
	PSECURITY_DESCRIPTOR psd;

	if (!win_acl_get_fd(Device->Handle, &psd)){
		return(FALSE);
	}else if (!GetFileInformationByHandle(Device->Handle, (BY_HANDLE_FILE_INFORMATION *)Result)){
		WIN_ERR("GetFileInformationByHandle(%d): %s\n", Device->Handle, win_strerror(GetLastError()));
	}else if (vfs_acl_stat(psd, Result)){
		Result->DeviceId = __Mounts->DeviceId;
		Result->Mode.FileType = Device->FileType;
		Result->SpecialId = Device->DeviceId;
		bResult = TRUE;
	}
	LocalFree(psd);
	return(bResult);
}
