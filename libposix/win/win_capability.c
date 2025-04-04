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

#define TOKEN_ADJUST_SESSIONID		0x100
#define CAP_ACESIZE			(sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD))

typedef struct _WIN_CAP_CONTROL {
	LUID AuthId;
	SID8 User;
	SID8 Primary;
	DWORD SessionId;
	PTOKEN_GROUPS Groups;
	PTOKEN_PRIVILEGES Privs;
//	PTOKEN_DEFAULT_DACL Default;
	ACL DefaultACL;
	ACCESS_ALLOWED_ACE8 Ace[4];
} WIN_CAP_CONTROL;

/* The "Replace a process level token" (SeAssignPrimaryTokenPrivilege) setting
 * determines which user accounts can call the CreateProcessAsUser() application
 * programming interface (API) so that one service can start another.
 * The "Create a token object" (SeCreateTokenPrivilege) is needed for NtCreateToken().
 * The "Act as part of the operating system" (SeTcbPrivilege) is needed for
 * assigning a TokenSessionId.
 */

/************************************************************/

BOOL 
CapCreateToken(WIN_CAP_CONTROL *Control, HANDLE *Result)
{
	BOOL bResult = FALSE;
	SECURITY_QUALITY_OF_SERVICE sqos = {sizeof(sqos), SecurityImpersonation, SECURITY_STATIC_TRACKING, TRUE};
	OBJECT_ATTRIBUTES oa = {sizeof(oa), NULL, NULL, 0, NULL, &sqos};
	TOKEN_OWNER tOwner = {&Control->User};
	TOKEN_USER tUser = {&Control->User, SE_GROUP_ENABLED};
	TOKEN_PRIMARY_GROUP tPrimaryGroup = {&Control->Primary};
	TOKEN_DEFAULT_DACL tDefaultACL = {&Control->DefaultACL};
	LARGE_INTEGER liExpire = {0xffffffff, 0x7fffffff};
	HANDLE hToken, hResult;
	NTSTATUS ntStatus;
	ACCESS_MASK aMask = TOKEN_ALL_ACCESS | TOKEN_ADJUST_SESSIONID;
	TOKEN_SOURCE tSource = {"MinC", {6, 1}};

	ntStatus = NtCreateToken(&hToken, aMask, &oa, TokenPrimary, &Control->AuthId, &liExpire, &tUser, 
		Control->Groups, Control->Privs, &tOwner, &tPrimaryGroup, &tDefaultACL, &tSource);
	if (!NT_SUCCESS(ntStatus)){
		WIN_ERR("NtCreateToken(%s): %s\n", win_strsid(&Control->User), nt_strerror(ntStatus));
	}else if (!SetTokenInformation(hToken, TokenSessionId, &Control->SessionId, sizeof(DWORD))){
		WIN_ERR("SetTokenInformation(TokenSessionId(%d)): %s\n", Control->SessionId, win_strerror(GetLastError()));
	}else if (!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenImpersonation, &hResult)){
		WIN_ERR("DuplicateToken(%s): %s\n", win_strsid(&Control->User), win_strerror(GetLastError()));
	}else{
		*Result = hResult;
		bResult = CloseHandle(hToken);
	}
	return(bResult);
}
SID_AND_ATTRIBUTES *
CapAddGroups(SID_AND_ATTRIBUTES Entry[], ULONG SubAuth, SID8 Groups[], DWORD Count, DWORD *Result)
{
	DWORD dwIndex = 0;
//	DWORD dwAttribs = SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT;

	while (dwIndex < Count){
		if (Groups->SubAuthority[0] == SubAuth){
			Entry->Sid = Groups;
			Entry->Attributes = SE_GROUP_ENABLED | SE_GROUP_OWNER;
			*Result += 1;
			Entry++;
		}
		dwIndex++;
		Groups++;
	}
	return(Entry);
}
PTOKEN_GROUPS 
CapMergeGroups(PTOKEN_GROUPS Source, SID8 Groups[], DWORD Count)
{
	DWORD dwCount = Source->GroupCount + Count;
	DWORD dwSize = sizeof(DWORD) + (dwCount * sizeof(SID_AND_ATTRIBUTES));
	PTOKEN_GROUPS ptResult = LocalAlloc(LMEM_FIXED, dwSize);
	PSID_AND_ATTRIBUTES ptEntry = ptResult->Groups;
	DWORD dwIndex = 0;
	DWORD dwResult = 0;
	ULONG ulSubAuth, ulLast = -1;
	BYTE bAuth;
	SID8 *pSid;

	while (dwIndex < Source->GroupCount){
		pSid = Source->Groups[dwIndex].Sid;
		bAuth = pSid->IdentifierAuthority.Value[5];
		ulSubAuth = pSid->SubAuthority[0];
		if (EqualPrefixSid(pSid, &SidNTAuthority)){
			*ptEntry = Source->Groups[dwIndex];
			dwResult++;
			ptEntry++;
		}else if (ulSubAuth != ulLast){
			switch (ulSubAuth){
				case SECURITY_MACHINE_DOMAIN_RID:
				case SECURITY_BUILTIN_DOMAIN_RID:
					ptEntry = CapAddGroups(ptEntry, ulSubAuth, Groups, Count, &dwResult);
					break;
				default:
					*ptEntry = Source->Groups[dwIndex];
					dwResult++;
					ptEntry++;
			}
		}
		dwIndex++;
		ulLast = ulSubAuth;
	}
	ptResult->GroupCount = dwResult;
	return(ptResult);
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
CapGetUser(HANDLE Token, SID8 *Sid)
{
	BOOL bResult = FALSE;
	SID8 __buf;
	TOKEN_USER tUser;
	DWORD dwSize = sizeof(TOKEN_USER) + sizeof(__buf);

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
	BOOL bResult = FALSE;
	LONG lSize = 0;

	GetTokenInformation(Token, TokenGroups, NULL, lSize, &lSize);
	if (lSize > 0){
		*Result = LocalAlloc(LMEM_FIXED, lSize);
		bResult = GetTokenInformation(Token, TokenGroups, *Result, lSize, &lSize);
	}else{
		WIN_ERR("GetTokenInformation(TokenGroups): %s\n", win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
CapGetPrivileges(HANDLE Token, PTOKEN_PRIVILEGES *Result)
{
	BOOL bResult = FALSE;
	LONG lSize = 0;

	GetTokenInformation(Token, TokenPrivileges, NULL, lSize, &lSize);
	if (lSize > 0){
		*Result = LocalAlloc(LMEM_FIXED, lSize);
		bResult = GetTokenInformation(Token, TokenPrivileges, *Result, lSize, &lSize);
	}else{
		WIN_ERR("GetTokenInformation(TokenPrivileges): %s\n", win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
CapGetDefault(HANDLE Token, PTOKEN_DEFAULT_DACL *Result)
{
	BOOL bResult = FALSE;
	LONG lSize = 0;

	GetTokenInformation(Token, TokenDefaultDacl, NULL, lSize, &lSize);
	if (lSize > 0){
		*Result = LocalAlloc(LMEM_FIXED, lSize);
		bResult = GetTokenInformation(Token, TokenDefaultDacl, *Result, lSize, &lSize);
	}else{
		WIN_ERR("GetTokenInformation(TokenDefaultDacl): %s\n", win_strerror(GetLastError()));
	}
	return(bResult);
}
BOOL 
CapGetRealToken(DWORD Access, TOKEN_TYPE Type, HANDLE *Result)
{
	BOOL bResult = FALSE;
	HANDLE hProcToken;

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
BOOL 
CapTogglePrivilege(HANDLE Token, LPCSTR Name, DWORD Value)
{
	BOOL bResult = FALSE;
	TOKEN_PRIVILEGES tPriv = {1, {0, 0, Value}};

	if (LookupPrivilegeValue(NULL, Name, &tPriv.Privileges[0].Luid)){
		if (!AdjustTokenPrivileges(Token, FALSE, &tPriv, sizeof(TOKEN_PRIVILEGES), NULL, NULL)){
			WIN_ERR("AdjustTokenPrivileges(%s): %s\n", Name, win_strerror(GetLastError()));
		}else if (ERROR_SUCCESS == GetLastError()){
			bResult = TRUE;
//		}else{
//			WIN_ERR("AdjustTokenPrivileges(%s): %s\n", Name, win_strerror(GetLastError()));
		}
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
win_cap_set_mode(DWORD Group, DWORD Other, ACL *Result)
{
	Result->AclRevision = ACL_REVISION;
	Result->AclSize = sizeof(ACL);
	Result->AceCount = 0;
	Result->AclSize += CAP_ACESIZE + GetLengthSid(&SidAdmins);
	AddAccessAllowedAce(Result, ACL_REVISION, GENERIC_ALL, &SidAdmins);
	Result->AclSize += CAP_ACESIZE + GetLengthSid(&SidUsers);
	AddAccessAllowedAce(Result, ACL_REVISION, Group, &SidUsers);
	Result->AclSize += CAP_ACESIZE + GetLengthSid(&SidAuthenticated);
	AddAccessAllowedAce(Result, ACL_REVISION, Other, &SidAuthenticated);
	return(TRUE);
}
BOOL 
win_cap_init(PLUID AuthId, WIN_CAP_CONTROL *Result)
{
	BOOL bResult = FALSE;
	HANDLE hToken;
	DWORD dwSize = 0;
	DWORD dwSessionId = 0;
	NTSTATUS ntStatus;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)){
		WIN_ERR("OpenProcessToken(TOKEN_QUERY): %s\n", win_strerror(GetLastError()));
	}else if (!GetTokenInformation(hToken, TokenSessionId, &dwSessionId, sizeof(DWORD), &dwSize)){
		WIN_ERR("GetTokenInformation(TokenSessionId): %s\n", win_strerror(GetLastError()));
	}else{
		Result->AuthId = *AuthId;
		Result->SessionId = dwSessionId;
		bResult = CloseHandle(hToken);
	}
	return(bResult);
}
VOID 
win_cap_free(WIN_CAP_CONTROL *Control)
{
	LocalFree(Control->Privs);
	LocalFree(Control->Groups);
//	LocalFree(Control->Default);
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
win_cap_setuid(PLUID AuthId, WIN_PWENT *Passwd, HANDLE *Result)
{
	BOOL bResult = FALSE;
	HANDLE hToken = NULL;
	WIN_CAP_CONTROL wControl;
	DWORD dwAccess = TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY;

	if (!win_cap_get_proc(dwAccess, 0, &hToken)){
		return(FALSE);
	}else if (!CapGetPrimary(hToken, &wControl.Primary)){
		WIN_ERR("CapGetPrimary(%d): %s\n", hToken, win_strerror(GetLastError()));
	}else if (!CapGetGroups(hToken, &wControl.Groups)){
		WIN_ERR("CapGetGroups(%d): %s\n", hToken, win_strerror(GetLastError()));
	}else if (!CapGetPrivileges(hToken, &wControl.Privs)){
		WIN_ERR("CapGetPrivileges(%d): %s\n", hToken, win_strerror(GetLastError()));
//	}else if (!CapGetDefault(hToken, &wControl.Default)){
//		WIN_ERR("CapGetDefault(%d): %s\n", hToken, win_strerror(GetLastError()));
	}else if (win_cap_init(AuthId, &wControl)){

		wControl.User = Passwd->UserSid;
		win_cap_set_mode(WIN_P_IRWX, WIN_P_IRX, &wControl.DefaultACL);

		if (CapTogglePrivilege(hToken, "SeCreateTokenPrivilege", SE_PRIVILEGE_ENABLED)){
			if (CapTogglePrivilege(hToken, "SeTcbPrivilege", SE_PRIVILEGE_ENABLED)){
				bResult = CapCreateToken(&wControl, Result);
			}
		}

		win_cap_free(&wControl);

	}
	CloseHandle(hToken);
	return(bResult);
}
BOOL 
win_cap_setgid(SID8 *Group)
{
	BOOL bResult = FALSE;
	TOKEN_PRIMARY_GROUP tPrimaryGroup = {Group};
	HANDLE hToken = NULL;
	DWORD dwAccess = TOKEN_ADJUST_PRIVILEGES | TOKEN_ADJUST_DEFAULT;

	if (!win_cap_get_proc(dwAccess, 0, &hToken)){
		return(FALSE);
	}else if (CapTogglePrivilege(hToken, "SeTcbPrivilege", SE_PRIVILEGE_ENABLED)){
		bResult = SetTokenInformation(hToken, TokenPrimaryGroup, &tPrimaryGroup, sizeof(TOKEN_PRIMARY_GROUP));
//		CapTogglePrivilege(hToken, "SeTcbPrivilege", 0);
	}
	CloseHandle(hToken);
	return(bResult);
}
BOOL 
win_cap_setgroups(PLUID AuthId, SID8 Groups[], DWORD Count, HANDLE *Result)
{
	BOOL bResult = FALSE;
	WIN_CAP_CONTROL wControl;
	HANDLE hToken = NULL;
	PTOKEN_GROUPS ptGroups;
	DWORD dwAccess = TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY;

	if (!win_cap_get_proc(dwAccess, 0, &hToken)){
		return(FALSE);
	}else if (!CapGetUser(hToken, &wControl.User)){
		WIN_ERR("CapGetUser(): %s\n", win_strerror(GetLastError()));
	}else if (!CapGetPrivileges(hToken, &wControl.Privs)){
		WIN_ERR("CapGetPrivileges(): %s\n", win_strerror(GetLastError()));
	}else if (!CapGetPrimary(hToken, &wControl.Primary)){
		WIN_ERR("CapGetPrimary(%d): %s\n", hToken, win_strerror(GetLastError()));
	}else if (!CapGetGroups(hToken, &ptGroups)){
		WIN_ERR("CapGetGroups(%d): %s\n", hToken, win_strerror(GetLastError()));
//	}else if (!CapGetDefault(hToken, &wControl.Default)){
//		WIN_ERR("CapGetDefault(%d): %s\n", hToken, win_strerror(GetLastError()));
	}else if (win_cap_init(AuthId, &wControl)){

		wControl.Groups = CapMergeGroups(ptGroups, Groups, Count);
		win_cap_set_mode(WIN_P_IRWX, WIN_P_IRX, &wControl.DefaultACL);

		if (CapTogglePrivilege(hToken, "SeCreateTokenPrivilege", SE_PRIVILEGE_ENABLED)){
			if (CapTogglePrivilege(hToken, "SeTcbPrivilege", SE_PRIVILEGE_ENABLED)){
				bResult = CapCreateToken(&wControl, Result);
			}
		}

		win_cap_free(&wControl);
		LocalFree(ptGroups);

	}
	CloseHandle(hToken);
	return(bResult);
}
