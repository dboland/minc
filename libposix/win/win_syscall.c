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

SID8 *
win_getuid(SID8 *Sid)
{
	HANDLE hToken;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)){
		WIN_ERR("win_getuid(OpenProcessToken(TOKEN_QUERY)): %s\n", win_strerror(GetLastError()));
	}else{
		CapGetUser(hToken, Sid);
		CloseHandle(hToken);
	}
	return(Sid);
}
SID8 *
win_geteuid(SID8 *Sid)
{
	HANDLE hToken;

	if (OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &hToken)){
		CapGetUser(hToken, Sid);
		CloseHandle(hToken);
	}else if (ERROR_NO_TOKEN == GetLastError()){
		win_getuid(Sid);
	}else{
		WIN_ERR("win_geteuid(OpenThreadToken(TOKEN_QUERY)): %s\n", win_strerror(GetLastError()));
	}
	return(Sid);
}
SID8 *
win_getgid(SID8 *Sid)
{
	HANDLE hToken;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)){
		WIN_ERR("win_getgid(OpenProcessToken(TOKEN_QUERY)): %s\n", win_strerror(GetLastError()));
	}else{
		CapGetPrimary(hToken, Sid);
		CloseHandle(hToken);
	}
	return(Sid);
}
SID8 *
win_getegid(SID8 *Sid)
{
	HANDLE hToken;

	if (OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &hToken)){
		CapGetPrimary(hToken, Sid);
		CloseHandle(hToken);
	}else if (ERROR_NO_TOKEN == GetLastError()){
		win_getgid(Sid);
	}else{
		WIN_ERR("win_getegid(OpenThreadToken(TOKEN_QUERY)): %s\n", win_strerror(GetLastError()));
	}
	return(Sid);
}

/************************************************************/

BOOL 
win_group_member(PSID Group)
{
	BOOL bResult = FALSE;

	if (!CheckTokenMembership(NULL, Group, &bResult)){
		WIN_ERR("CheckTokenMembership(): %s\n", win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
win_getgroups(SID8 *Groups[], DWORD *Count)
{
	BOOL bResult = FALSE;
	PTOKEN_GROUPS ptGroups;
	PSID_AND_ATTRIBUTES psaGroups;
	HANDLE hToken;
	SID8 *psResult;
	DWORD dwCount;
	DWORD dwIndex = 0;
	DWORD dwResult = 0;
	PSID pSid;

	if (!win_cap_get_proc(TOKEN_QUERY, 0, &hToken)){
		return(FALSE);
	}else if (CapGetGroups(hToken, &ptGroups)){
		dwCount = ptGroups->GroupCount;
		psResult = win_malloc(sizeof(SID8) * dwCount);
		psaGroups = ptGroups->Groups;
		while (dwIndex < dwCount){
			pSid = psaGroups->Sid;
			if (psaGroups->Attributes & SE_GROUP_ENABLED){
				CopySid(GetLengthSid(pSid), &psResult[dwResult++], pSid);
			}
			dwIndex++;
			psaGroups++;
		}
		LocalFree(ptGroups);
		*Groups = psResult;
		*Count = dwResult;
		bResult = CloseHandle(hToken);
	}
	return(bResult);
}
BOOL 
win_setgroups(WIN_CAP_CONTROL *Control, SID8 Groups[], DWORD Count)
{
	BOOL bResult = FALSE;
	HANDLE hToken;

	if (!win_cap_setgroups(Control, Groups, Count, &hToken)){
		return(FALSE);
	}else if (!SetThreadToken(NULL, hToken)){
		WIN_ERR("SetThreadToken(): %s\n", win_strerror(GetLastError()));
	}else{
		bResult = CloseHandle(hToken);
	}
	return(bResult);
}
BOOL 
win___tfork_thread(WIN___TFORK *Params, SIZE_T Size, LPTHREAD_START_ROUTINE Start, PVOID Data, DWORD *Result)
{
	BOOL bResult = FALSE;
	HANDLE hResult = NULL;

	if (!(hResult = CreateThread(NULL, Size, Start, Data, 0, Result))){
		WIN_ERR("CreateThread(%d): %s\n", Size, win_strerror(GetLastError()));
	}else{
		bResult = CloseHandle(hResult);
	}
	return(bResult);
}
