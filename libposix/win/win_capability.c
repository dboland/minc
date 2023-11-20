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

#include <ddk/ntapi.h>

typedef struct _WIN_CAP_CONTROL {
	SID8 User;
	SID8 Primary;
	LUID AuthId;
	DWORD SessionId;
	TOKEN_SOURCE Source;
	PTOKEN_GROUPS Groups;
	PTOKEN_PRIVILEGES Privs;
	ACL DefaultAcl;
	ACCESS_ALLOWED_ACE8 Ace[3];
} WIN_CAP_CONTROL;

/************************************************************/

BOOL 
CapCreateToken(WIN_CAP_CONTROL *Control, HANDLE *Result)
{
	BOOL bResult = FALSE;
	SECURITY_QUALITY_OF_SERVICE sqos = {sizeof(sqos), SecurityImpersonation, SECURITY_STATIC_TRACKING, TRUE};
	OBJECT_ATTRIBUTES oa = {sizeof(oa), NULL, NULL, 0, NULL, &sqos};
	TOKEN_OWNER tOwner = {&Control->User};
	TOKEN_USER tUser = {&Control->User, 0};
	TOKEN_PRIMARY_GROUP tPrimaryGroup = {&Control->Primary};
	TOKEN_DEFAULT_DACL tDefaultACL = {&Control->DefaultAcl};
	LARGE_INTEGER liExpire = {0xffffffff, 0x7fffffff};
	HANDLE hToken, hResult;
	NTSTATUS ntStatus;

	ntStatus = NtCreateToken(&hToken, TOKEN_ALL_ACCESS, &oa, TokenPrimary, &Control->AuthId, &liExpire, &tUser, 
		Control->Groups, Control->Privs, &tOwner, &tPrimaryGroup, &tDefaultACL, &Control->Source);
	if (!NT_SUCCESS(ntStatus)){
		WIN_ERR("NtCreateToken(%s): %s\n", win_strsid(&Control->User), nt_strerror(ntStatus));
	}else if (!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenImpersonation, &hResult)){
		WIN_ERR("DuplicateToken(%s): %s\n", win_strsid(&Control->User), win_strerror(GetLastError()));
	}else{
		*Result = hResult;
		bResult = CloseHandle(hToken);
	}
	return(bResult);
}
PTOKEN_GROUPS 
CapCreateGroups(SID8 Groups[], DWORD Count)
{
	DWORD dwAttribs = SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT;
	DWORD dwSize = (Count * sizeof(SID_AND_ATTRIBUTES)) + sizeof(DWORD);
	PTOKEN_GROUPS ptGroups = win_malloc(dwSize);
	DWORD dwIndex = 0;

	while (dwIndex < Count){
		if (EqualSid(&Groups[dwIndex], &SidAdmins)){
			ptGroups->Groups[dwIndex].Attributes = dwAttribs | SE_GROUP_OWNER;
		}else{
			ptGroups->Groups[dwIndex].Attributes = dwAttribs | SE_GROUP_MANDATORY;
		}
		ptGroups->Groups[dwIndex].Sid = &Groups[dwIndex];
		dwIndex++;
	}
	ptGroups->GroupCount = Count;
	return(ptGroups);
}
PTOKEN_PRIVILEGES 
CapAddPrivilege(PTOKEN_PRIVILEGES Privs, LPCSTR Name)
{
	DWORD dwAttribs = SE_PRIVILEGE_ENABLED + SE_PRIVILEGE_ENABLED_BY_DEFAULT;
	DWORD dwCount = Privs->PrivilegeCount;
	DWORD dwSize = (dwCount * sizeof(LUID_AND_ATTRIBUTES)) + sizeof(DWORD);
	PTOKEN_PRIVILEGES ptResult = Privs;
	LUID luid;

	if (!LookupPrivilegeValue(NULL, Name, &luid)){
		WIN_ERR("LookupPrivilegeValue(%s): %s\n", Name, win_strerror(GetLastError()));
	}else{
		dwSize += sizeof(LUID_AND_ATTRIBUTES);
		ptResult = win_realloc(Privs, dwSize);
		ptResult->Privileges[dwCount].Attributes = dwAttribs;
		ptResult->Privileges[dwCount].Luid = luid;
		ptResult->PrivilegeCount++;
	}
	return(ptResult);
}
BOOL 
CapTogglePrivilege(HANDLE Token, LPCSTR Name, DWORD Value)
{
	TOKEN_PRIVILEGES tPriv = {1, {0, 0, Value}};
	BOOL bResult = FALSE;

	if (!LookupPrivilegeValue(NULL, Name, &tPriv.Privileges[0].Luid)){
		return(FALSE);
	}else if (!AdjustTokenPrivileges(Token, FALSE, &tPriv, sizeof(tPriv), NULL, NULL)){
		WIN_ERR("AdjustTokenPrivileges(%s): %s\n", Name, win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
CapGetUser(HANDLE Token, SID8 *Sid)
{
	SID8 __buf;
	TOKEN_USER tUser;
	DWORD dwSize = sizeof(TOKEN_USER) + sizeof(__buf);
	BOOL bResult = FALSE;

	if (!GetTokenInformation(Token, TokenUser, &tUser, dwSize, &dwSize)){
		WIN_ERR("GetTokenInformation(TokenUser): %s\n", win_strerror(GetLastError()));
	}else{
		bResult = CopySid(GetLengthSid(tUser.User.Sid), Sid, tUser.User.Sid);
	}
	return(bResult);
}
BOOL 
CapGetPrimary(HANDLE Token, SID8 *Sid)
{
	BOOL bResult = FALSE;
	SID8 __buf;
	TOKEN_PRIMARY_GROUP tGroup;
	DWORD dwSize = sizeof(TOKEN_PRIMARY_GROUP) + sizeof(__buf);

	if (!GetTokenInformation(Token, TokenPrimaryGroup, &tGroup, dwSize, &dwSize)){
		WIN_ERR("GetTokenInformation(TokenPrimaryGroup): %s\n", win_strerror(GetLastError()));
	}else{
		bResult = CopySid(GetLengthSid(tGroup.PrimaryGroup), Sid, tGroup.PrimaryGroup);
	}
	return(bResult);
}
BOOL 
CapGetGroups(HANDLE Token, PTOKEN_GROUPS *Result)
{
	DWORD dwSize = 0;
	BOOL bResult = FALSE;

	GetTokenInformation(Token, TokenGroups, NULL, dwSize, &dwSize);
	if (dwSize){
		*Result = win_malloc(dwSize);
		bResult = GetTokenInformation(Token, TokenGroups, *Result, dwSize, &dwSize);
	}else{
		WIN_ERR("GetTokenInformation(TokenGroups): %s\n", win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
CapGetPrivileges(HANDLE Token, PTOKEN_PRIVILEGES *Result)
{
	DWORD dwSize = 0;
	BOOL bResult = FALSE;

	GetTokenInformation(Token, TokenPrivileges, NULL, dwSize, &dwSize);
	if (dwSize){
		*Result = win_malloc(dwSize);
		bResult = GetTokenInformation(Token, TokenPrivileges, *Result, dwSize, &dwSize);
	}else{
		WIN_ERR("GetTokenInformation(TokenPrivileges): %s\n", win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
CapGetDefaultAcl(HANDLE Token, PTOKEN_DEFAULT_DACL *Result)
{
	DWORD dwSize = 0;
	BOOL bResult = FALSE;

	GetTokenInformation(Token, TokenDefaultDacl, NULL, dwSize, &dwSize);
	if (dwSize){
		*Result = win_malloc(dwSize);
		bResult = GetTokenInformation(Token, TokenDefaultDacl, *Result, dwSize, &dwSize);
	}else{
		WIN_ERR("GetTokenInformation(TokenDefaultDacl): %s\n", win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
CapGetRealToken(DWORD Access, TOKEN_TYPE Type, HANDLE *Result)
{
	HANDLE hProcToken;
	BOOL bResult = FALSE;

	if (!OpenProcessToken(GetCurrentProcess(), Access, &hProcToken)){
		WIN_ERR("CapGetRealToken(%d): %s\n", GetCurrentProcess(), win_strerror(GetLastError()));
	}else if (Type != TokenImpersonation){
		*Result = hProcToken;
		bResult = TRUE;
	}else if (DuplicateTokenEx(hProcToken, Access, NULL, SecurityImpersonation, TokenImpersonation, Result)){
		bResult = CloseHandle(hProcToken);
	}
	return(bResult);
}

/************************************************************/

BOOL 
win_cap_get_proc(DWORD Access, TOKEN_TYPE Type, HANDLE *Result)
{
	HANDLE hThreadToken;
	BOOL bResult = TRUE;

	if (!OpenThreadToken(GetCurrentThread(), Access, TRUE, &hThreadToken)){
		bResult = CapGetRealToken(Access, Type, Result);
	}else if (Type != TokenPrimary){
		*Result = hThreadToken;
	}else if (DuplicateTokenEx(hThreadToken, Access, NULL, SecurityImpersonation, TokenPrimary, Result)){
		bResult = CloseHandle(hThreadToken);
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
BOOL 
win_cap_set_mode(SID8 *User, ACCESS_MASK Access, ACL *Result)
{
	DWORD aceSize = sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD);

	Result->AclRevision = ACL_REVISION;
	Result->AclSize = sizeof(ACL);
	Result->AceCount = 0;
	Result->AclSize += aceSize + GetLengthSid(&SidAdmins);
	AddAccessAllowedAce(Result, ACL_REVISION, GENERIC_ALL, &SidAdmins);
	Result->AclSize += aceSize + GetLengthSid(User);
	AddAccessAllowedAce(Result, ACL_REVISION, GENERIC_ALL, User);
	Result->AclSize += aceSize + GetLengthSid(&SidAuthenticated);
	AddAccessAllowedAce(Result, ACL_REVISION, Access, &SidAuthenticated);
	return(TRUE);
}
BOOL 
win_cap_init(WIN_CAP_CONTROL *Result)
{
	BOOL bResult = FALSE;
	HANDLE hToken;
	DWORD dwSize = 0;
	TOKEN_STATISTICS tStats = {0};
	TOKEN_SOURCE tSource = {0};
	DWORD dwSessionId = 0;
	NTSTATUS ntStatus;

	if (!OpenProcessToken(GetCurrentProcess(), MAXIMUM_ALLOWED, &hToken)){
//		WIN_ERR("OpenProcessToken(): %s\n", win_strerror(GetLastError()));
		SetLastError(ERROR_PRIVILEGE_NOT_HELD);
	}else if (!GetTokenInformation(hToken, TokenSource, &tSource, sizeof(TOKEN_SOURCE), &dwSize)){
		WIN_ERR("GetTokenInformation(TokenSource): %s\n", win_strerror(GetLastError()));
	}else if (!GetTokenInformation(hToken, TokenStatistics, &tStats, sizeof(TOKEN_STATISTICS), &dwSize)){
		WIN_ERR("GetTokenInformation(TokenStatistics): %s\n", win_strerror(GetLastError()));
	}else if (!GetTokenInformation(hToken, TokenSessionId, &dwSessionId, sizeof(DWORD), &dwSize)){
		WIN_ERR("GetTokenInformation(TokenSessionId): %s\n", win_strerror(GetLastError()));
	}else{
		Result->AuthId = tStats.AuthenticationId;
		Result->SessionId = dwSessionId;
		Result->Source = tSource;
		bResult = CloseHandle(hToken);
	}
	return(bResult);
}
VOID 
win_cap_free(WIN_CAP_CONTROL *Control)
{
	win_free(Control->Privs);
	win_free(Control->Groups);
}
BOOL 
win_cap_from_name(LPCSTR Name, PTOKEN_PRIVILEGES Token)
{
	BOOL bResult = FALSE;

	if (!LookupPrivilegeValue(NULL, Name, &Token->Privileges[0].Luid)){
		WIN_ERR("LookupPrivilegeValue(%s): %s\n", Name, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
win_cap_to_name(PTOKEN_PRIVILEGES Token, LPSTR Buffer, DWORD *Size)
{
	BOOL bResult = FALSE;

	if (!LookupPrivilegeName("", &Token->Privileges[0].Luid, Buffer, Size)){
		WIN_ERR("LookupPrivilegeName(): %s\n", win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
win_cap_setuid(WIN_PWENT *Passwd, HANDLE *Result)
{
	BOOL bResult = FALSE;
	HANDLE hToken = NULL;
	WIN_CAP_CONTROL wControl;

	if (!win_cap_get_proc(MAXIMUM_ALLOWED, 0, &hToken)){
		return(FALSE);
	}else if (!CapGetPrimary(hToken, &wControl.Primary)){
		WIN_ERR("CapGetPrimary(%d): %s\n", hToken, win_strerror(GetLastError()));
	}else if (!CapGetGroups(hToken, &wControl.Groups)){
		WIN_ERR("CapGetGroups(%d): %s\n", hToken, win_strerror(GetLastError()));
	}else if (!CapGetPrivileges(hToken, &wControl.Privs)){
		WIN_ERR("CapGetPrivileges(%d): %s\n", hToken, win_strerror(GetLastError()));
	}else if (win_cap_init(&wControl)){

		wControl.User = Passwd->UserSid;

		if (Passwd->Integrity == SECURITY_MANDATORY_SYSTEM_RID){
			wControl.Privs = CapAddPrivilege(wControl.Privs, "SeTcbPrivilege");
		/* Vista */
//		}else if (!SetTokenInformation(hToken, TokenSessionId, &wControl.SessionId, sizeof(DWORD))){
//			WIN_ERR("SetTokenInformation(TokenSessionId(%d)): %s\n", wControl.SessionId, win_strerror(GetLastError()));
		}

		win_cap_set_mode(&wControl.User, GENERIC_ALL, &wControl.DefaultAcl);

		CapTogglePrivilege(hToken, "SeCreateTokenPrivilege", SE_PRIVILEGE_ENABLED);
		/* Vista */
		CapTogglePrivilege(hToken, "SeDelegateSessionUserImpersonatePrivilege", SE_PRIVILEGE_ENABLED);

		bResult = CapCreateToken(&wControl, Result);

		win_cap_free(&wControl);

	}
	CloseHandle(hToken);
	return(bResult);
}
BOOL 
win_cap_setgroups(SID8 *Primary, SID8 Groups[], DWORD Count, HANDLE *Result)
{
	BOOL bResult = FALSE;
	WIN_CAP_CONTROL wControl;
	HANDLE hToken = NULL;

	if (!win_cap_get_proc(MAXIMUM_ALLOWED, 0, &hToken)){
		return(FALSE);
	}else if (!CapGetUser(hToken, &wControl.User)){
		WIN_ERR("CapGetUser(): %s\n", win_strerror(GetLastError()));
	}else if (!CapGetPrivileges(hToken, &wControl.Privs)){
		WIN_ERR("CapGetPrivileges(): %s\n", win_strerror(GetLastError()));
	}else if (win_cap_init(&wControl)){

		wControl.Groups = CapCreateGroups(Groups, Count);
		wControl.Primary = *Primary;

		win_cap_set_mode(&wControl.User, GENERIC_ALL, &wControl.DefaultAcl);

		CapTogglePrivilege(hToken, "SeCreateTokenPrivilege", SE_PRIVILEGE_ENABLED);
		CapTogglePrivilege(hToken, "SeDelegateSessionUserImpersonatePrivilege", SE_PRIVILEGE_ENABLED);

		bResult = CapCreateToken(&wControl, Result);

		win_cap_free(&wControl);

	}
	CloseHandle(hToken);
	return(bResult);
}
