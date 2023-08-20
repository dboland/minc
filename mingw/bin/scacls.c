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

#include <windows.h>
#include "win_types.h"

#include <stdio.h>
#include <strings.h>
#include <errno.h>
#include <stdlib.h>

#define WIN_ERR(...)	fprintf(stderr, __VA_ARGS__)
#define msvc_sprintf	sprintf

char _NOSID[] = {'N', 'o', 'n', 'e', 0};
char _ERR255[255];

BOOL _DEBUG;

/******************************************************************************/

LPSTR 
win_strerrorat(HMODULE Module, LPCSTR Label, DWORD Error)
{
	DWORD dwFlags = FORMAT_MESSAGE_FROM_SYSTEM + FORMAT_MESSAGE_IGNORE_INSERTS 
		+ FORMAT_MESSAGE_MAX_WIDTH_MASK;
	DWORD dwLanguageID = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
	LPSTR pszError = _ERR255;

	if (Module){
		dwFlags += FORMAT_MESSAGE_FROM_HMODULE;
	}
	pszError += msvc_sprintf(pszError, Label, Error);
	if (!FormatMessage(dwFlags, Module, Error, dwLanguageID, pszError, 240, NULL)){
		msvc_sprintf(pszError, "Unknown error ");
	}
	return(_ERR255);
}
LPSTR 
win_strerror(DWORD Error)
{
	return(win_strerrorat(NULL, "Error(%d): ", Error));
}
BOOL 
win_execve(LPSTR Command)
{
	BOOL bResult = FALSE;
	PROCESS_INFORMATION pi = {0};
	STARTUPINFO si = {0};
	DWORD dwIndex = 0;

	si.cb = sizeof(STARTUPINFO);
	si.lpDesktop = "";			/* Vista */
	if (CreateProcess(NULL, Command, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi)){
		CloseHandle(pi.hThread);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		bResult = TRUE;
	}else{
		WIN_ERR("CreateProcess(%s): %s\n", Command, win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
win_sidname(SID8 *Sid, LPSTR Buffer, DWORD Length)
{
	BOOL bResult = FALSE;
	DWORD sidLen = sizeof(SID8);
	CHAR szDomain[MAX_NAME];
	DWORD dwLength = MAX_NAME;
	SID_NAME_USE snType = 0;

	if (!LookupAccountSid(NULL, Sid, Buffer, &Length, szDomain, &dwLength, &snType)){
		WIN_ERR("LookupAccountSid(): %s\n", win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
PVOID 
win_malloc(ULONG Size)
{
	ULONG ulSize = MIN_BUFSIZE;
	HLOCAL hLocal;

	while (ulSize < Size){
		ulSize += MIN_BUFSIZE;
	}
	hLocal = LocalAlloc(LMEM_FIXED + LMEM_ZEROINIT, ulSize + sizeof(ULONG));
	*(ULONG *)hLocal = ulSize;
	return(hLocal + sizeof(ULONG));
}
PVOID 
win_realloc(PVOID Buffer, ULONG Size)
{
	HLOCAL hLocal = Buffer - sizeof(ULONG);
	ULONG ulSize = *(ULONG *)hLocal;

	if (Size > ulSize){
		while (ulSize < Size){
			ulSize += MIN_BUFSIZE;
		}
		/* I know this was allocated as LMEM_FIXED, but I would like you 
		 * to move it anyway. Trust me on this. (Raymond Chen, 2013)
		 */
		hLocal = LocalReAlloc(hLocal, ulSize + sizeof(ULONG), LMEM_MOVEABLE);
		*(ULONG *)hLocal = ulSize;
	}
	return(hLocal + sizeof(ULONG));
}
VOID 
win_free(PVOID Buffer)
{
	LocalFree(Buffer - sizeof(ULONG));
}

/******************************************************************************/

char *
stpcpy(char *dest, char *src)
{
	while (*dest = *src++){
		dest++;
	}
	return(dest);
}
char *
stpquot(char *dest, const char *src)
{
	char c;
	char count = 1;

	*dest++ = ' ';
	*dest++ = '"';
	while (c = *src++){
		if (c == '"'){
			while (count--){
				*dest++ = '\\';
			}
			count = 1;
		}else if (c == '\\'){
			count++;
		}else{
			count = 1;
		}
		*dest++ = c;
	}
	*dest++ = '"';
	*dest = 0;
	return(dest);
}
LPSTR 
parse_sid(LPSTR Result, const char *buffer)
{
	char c;
	char *buf = (char *)buffer;
	BYTE bCount = 0;
	ULONG ulValue;
	BYTE bRevision = 0;
	BYTE bAuth = 0;
	SID8 sid = {0};

	while (c = *buf++){
		if (bCount >= 8){
			break;
		}else if (c == '-'){
			ulValue = strtol(buf, &buf, 10);
			if (!bRevision){
				bRevision = ulValue;
			}else if (!bAuth){
				bAuth = ulValue;
			}else{
				sid.SubAuthority[bCount] = ulValue;
				bCount++;
			}
		}else if (c == ':'){		/* permissions */
			buf--;
			break;
		}
	}
	sid.Revision = bRevision;
	sid.IdentifierAuthority.Value[5] = bAuth;
	sid.SubAuthorityCount = bCount;
	if (win_sidname(&sid, Result, MAX_NAME)){
		strcat(Result, buf);		/* append permissions */
	}else{
		Result = _NOSID;
	}
	return(Result);
}
int 
main(int argc, const char *argv[])
{
	int result = -1;
	LPSTR pszCommand = win_malloc(MAX_PATH);
	char *p;
	int size = 0;
	int len;
	const char *arg = *argv++;		/* skip first argument (command) */
	CHAR szAccount[MAX_NAME];

	p = stpcpy(pszCommand, "cacls");
	size = p - pszCommand;
	while (arg = *argv++){
		if (!strncmp(arg, "S-1", 3)){
			arg = parse_sid(szAccount, arg);
		}else if (!strcmp(arg, "-d")){
			_DEBUG = TRUE;
		}
		len = strlen(arg) + 3;
		pszCommand = win_realloc(pszCommand, size + len);
		p = pszCommand + size;
		p = stpquot(p, arg);
		size = p - pszCommand;
	}
	if (_DEBUG){
		printf("Command: %s\n", pszCommand);
	}else if (win_execve(pszCommand)){
		result = 0;
	}
	win_free(pszCommand);
	return(result);
}
