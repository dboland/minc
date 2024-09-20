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
pipe_fstat(WIN_VNODE *Node, WIN_VATTR *Result)
{
	BOOL bResult = FALSE;
	PSECURITY_DESCRIPTOR psd;
	FILETIME ftNow;

	if (!win_acl_get_fd(Node->Handle, &psd)){
		return(FALSE);
	}else if (vfs_acl_stat(psd, Result)){
		GetSystemTimeAsFileTime(&ftNow);
		Result->CreationTime = ftNow;
		Result->LastAccessTime = ftNow;
		Result->LastWriteTime = ftNow;
		Result->NumberOfLinks = 1;
		Result->DeviceId = __Mounts->DeviceId;
		Result->SpecialId = Node->DeviceId;
		Result->Mode.FileType = Node->FileType;
		bResult = TRUE;
	}
	LocalFree(psd);
	return(bResult);
}
BOOL 
pipe_stat(WIN_NAMEIDATA *Path, WIN_VATTR *Result)
{
	BOOL bResult = FALSE;

	if (VfsStatHandle(Path->Object, Result)){
		Result->DeviceId = __Mounts->DeviceId;
		Result->Mode.FileType = Path->FileType;
		Result->SpecialId = Path->DeviceId;
		bResult = CloseHandle(Path->Object);
	}
	return(bResult);
}
BOOL 
pipe_mknod(LPWSTR FileName, WIN_MODE *Mode, LPWSTR NtName)
{
	BOOL bResult = FALSE;
	PSECURITY_DESCRIPTOR psd;
	WIN_ACL_CONTROL wControl;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), &wControl.Security, FALSE};
	WCHAR szDirName[WIN_PATH_MAX];

	if (!win_acl_get_file(win_dirname(szDirName, FileName), &psd)){
		return(FALSE);
	}else if (!win_acl_init(Mode, &wControl)){
		WIN_ERR("win_acl_init(%ls): %s\n", szDirName, win_strerror(GetLastError()));
	}else if (vfs_acl_create(psd, Mode, 0, &wControl)){
		bResult = pipe_F_CREATE(FileName, Mode->FileType, &sa, NtName);
	}
	LocalFree(psd);
	win_acl_free(&wControl);
	return(bResult);
}
BOOL 
pipe_chmod(WIN_NAMEIDATA *Path, WIN_MODE *Mode)
{
	BOOL bResult = FALSE;
	SECURITY_INFORMATION siType = OWNER_SECURITY_INFORMATION + GROUP_SECURITY_INFORMATION + DACL_SECURITY_INFORMATION;
	PSECURITY_DESCRIPTOR psd;
	DWORD dwSize = 0;
	WIN_ACL_CONTROL wControl;

	if (!win_acl_get_fd(Path->Object, &psd)){
		return(FALSE);
	}else if (!win_acl_init(Mode, &wControl)){
		WIN_ERR("pipe_chmod(%d): %s\n", Path->Object, win_strerror(GetLastError()));
	}else if (!vfs_acl_chmod(psd, Mode, &wControl)){
		WIN_ERR("pipe_chmod(%d): %s\n", Path->Object, win_strerror(GetLastError()));
	}else if (SetUserObjectSecurity(Path->Object, &siType, &wControl.Security)){
		bResult = CloseHandle(Path->Object);
	}
	LocalFree(psd);
	win_acl_free(&wControl);
	return(bResult);
}
