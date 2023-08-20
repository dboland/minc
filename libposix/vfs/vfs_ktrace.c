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

/************************************************************/

LPSTR 
flagname_win(LPSTR Buffer, DWORD Flag, LPCSTR Name, DWORD Mask)
{
	if (Mask & Flag){
		Buffer += msvc_sprintf(Buffer, "[%s]", Name);
	}
	return(Buffer);
}

/************************************************************/

BOOL 
vfs_ktrace_SET(WIN_TASK *Task, WIN_NAMEIDATA *Path, PVOID Buffer, LONG Size)
{
	BOOL bResult = FALSE;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
	HANDLE hResult;
	DWORD dwCount;
	ACCESS_MASK amAccess = GENERIC_READ + GENERIC_WRITE;
	DWORD dwShare = FILE_SHARE_READ + FILE_SHARE_WRITE;

	hResult = CreateFileW(Path->Resolved, amAccess, dwShare, &sa, 
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hResult == INVALID_HANDLE_VALUE){
		return(FALSE);
	}else if (!WriteFile(hResult, Buffer, Size, &dwCount, NULL)){
		WIN_ERR("WriteFile(%d): %s\n", hResult, win_strerror(GetLastError()));
	}else{
		Task->Flags |= WIN_PS_TRACED;
		Task->TraceHandle = hResult;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_ktrace_CLEAR(WIN_TASK *Task)
{
	BOOL bResult = FALSE;

	if (!CloseHandle(Task->TraceHandle)){
		WIN_ERR("vfs_ktrace_CLEAR(CloseHandle(%d)): %s\n", Task->TraceHandle, win_strerror(GetLastError()));
	}else{
		Task->Flags &= ~WIN_PS_TRACED;
		Task->TraceHandle = NULL;
		bResult = TRUE;
	}
}
LPSTR 
vfs_ktrace_ATTRIBS(LPSTR Buffer, DWORD Attribs)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, "\nAttribs([0x%x]", Attribs);
	if (Attribs != -1){
		psz = flagname_win(psz, FILE_FLAG_BACKUP_SEMANTICS, "BACKUP_SEMANTICS", Attribs);
		psz = flagname_win(psz, FILE_FLAG_OVERLAPPED, "OVERLAPPED", Attribs);
		psz = flagname_win(psz, FILE_ATTRIBUTE_READONLY, "READONLY", Attribs);
		psz = flagname_win(psz, FILE_ATTRIBUTE_HIDDEN, "HIDDEN", Attribs);
		psz = flagname_win(psz, FILE_ATTRIBUTE_SYSTEM, "SYSTEM", Attribs);
		psz = flagname_win(psz, FILE_ATTRIBUTE_DIRECTORY, "DIRECTORY", Attribs);
		psz = flagname_win(psz, FILE_ATTRIBUTE_ARCHIVE, "ARCHIVE", Attribs);
		psz = flagname_win(psz, FILE_ATTRIBUTE_DEVICE, "DEVICE", Attribs);
		psz = flagname_win(psz, FILE_ATTRIBUTE_NORMAL, "NORMAL", Attribs);
		psz = flagname_win(psz, FILE_ATTRIBUTE_TEMPORARY, "TEMPORARY", Attribs);
		psz = flagname_win(psz, FILE_ATTRIBUTE_REPARSE_POINT, "REPARSE_POINT", Attribs);
		psz = flagname_win(psz, FILE_ATTRIBUTE_COMPRESSED, "COMPRESSED", Attribs);
		psz = flagname_win(psz, FILE_ATTRIBUTE_OFFLINE, "OFFLINE", Attribs);
	}
	*psz++ = ')';
	*psz = 0;
	return(psz);
}
LPSTR 
vfs_ktrace_FLAGS(LPSTR Buffer, DWORD Flags)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, " Flags([0x%x]", Flags);
	psz = flagname_win(psz, HANDLE_FLAG_INHERIT, "INHERIT", Flags);
	psz = flagname_win(psz, HANDLE_FLAG_PROTECT_FROM_CLOSE, "PROTECT_FROM_CLOSE", Flags);
	*psz++ = ')';
	*psz = 0;
	return(psz);
}

/************************************************************/

DWORD 
vfs_ktrace(WIN_VNODE *Node, LPSTR Buffer)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, "FileId(%d) Type(%s:%s) Handle(%d) Access(0x%x) CloEx(%d) DevType(0x%x) DevId(0x%x)", 
		Node->FileId, _FSType[Node->FSType], FType(Node->FileType), Node->Handle, Node->Access, Node->CloseExec, Node->DeviceType, Node->DeviceId);
	psz = vfs_ktrace_ATTRIBS(psz, Node->Attribs);
	psz = vfs_ktrace_FLAGS(psz, Node->Flags);
	return(psz - Buffer);
}
