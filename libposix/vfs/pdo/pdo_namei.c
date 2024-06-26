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
pdo_lookup(WIN_NAMEIDATA *Path, DWORD Flags)
{
	BOOL bResult = FALSE;
	WIN_INODE iNode;
	DWORD dwSize = sizeof(WIN_INODE);
	WIN_DEVICE *pwDevice;
	HANDLE hResult;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};

	hResult = CreateFileW(Path->Resolved, GENERIC_READ, FILE_SHARE_READ, 
		&sa, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, NULL);
	if (hResult == INVALID_HANDLE_VALUE){
		WIN_ERR("CreateFile(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else if (!ReadFile(hResult, &iNode, dwSize, &dwSize, NULL)){
		WIN_ERR("ReadFile(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else if (iNode.Magic != TypeNameVirtual){
		SetLastError(ERROR_BAD_DEVICE);
	}else{
		Path->DeviceId = iNode.DeviceId;
		Path->FileType = iNode.FileType;
		Path->FSType = FS_TYPE_PDO;
		Path->Attribs |= FILE_ATTRIBUTE_DEVICE;
		if (Flags & WIN_KEEPOBJECT){
			Path->Object = hResult;
		}else{
			CloseHandle(hResult);
		}
		bResult = TRUE;
//vfs_ktrace("pdo_lookup", STRUCT_NAMEI, Path);
	}
	return(bResult);
}
