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

#include <lmaccess.h>
#include <lmapibuf.h>		/* NetApiBufferFree() */
#include <lmerr.h>		/* NERR_Success */

/************************************************************/

BOOL 
PwdLookupNTAuth(WIN_PWENT *Entity)
{
	BOOL bResult = FALSE;
	LPUSER_INFO_3 lpuInfo = NULL;
	NET_API_STATUS naStatus;

	/* latency if !NULL */
	naStatus = NetUserGetInfo(NULL, Entity->Account, 3, (LPBYTE *)&lpuInfo);
	if (naStatus == NERR_Success){
		win_wcstombs(Entity->Password, lpuInfo->usri3_password, MAX_NAME);
		Entity->Privileges = lpuInfo->usri3_priv;
		win_wcscpy(Entity->Home, lpuInfo->usri3_home_dir);
		win_wcstombs(Entity->Comment, lpuInfo->usri3_full_name, MAX_COMMENT);
		Entity->Change = lpuInfo->usri3_password_expired;
		Entity->Expire = lpuInfo->usri3_acct_expires;
		NetApiBufferFree(lpuInfo);
		if (Entity->Privileges == USER_PRIV_ADMIN){
			Entity->GroupSid = SidAdmins;
			Entity->Integrity = SECURITY_MANDATORY_HIGH_RID;
		}else if (Entity->Privileges == USER_PRIV_USER){
			Entity->GroupSid = SidUsers;
			Entity->Integrity = SECURITY_MANDATORY_MEDIUM_RID;
		}else{
			Entity->GroupSid = SidBatch;
			Entity->Integrity = SECURITY_MANDATORY_LOW_RID;
		}
		bResult = TRUE;
	}else{
		SetLastError(naStatus);
	}
	return(bResult);
}
BOOL 
PwdLookup(WIN_PWENT *Entity)
{
	BOOL bResult = TRUE;
	BYTE bAuth = Entity->UserSid.IdentifierAuthority.Value[5];
	ULONG ulSubAuth = Entity->UserSid.SubAuthority[0];

	if (bAuth != SECURITY_NT_AUTHORITY_RID){
		return(FALSE);
	}else switch (ulSubAuth){
		case SECURITY_NT_NON_UNIQUE_RID:	/* 21 (Machine) */
		case SECURITY_BUILTIN_DOMAIN_RID:	/* 32 (Builtin) */
			bResult = PwdLookupNTAuth(Entity);
			break;
		case SECURITY_LOCAL_SYSTEM_RID:		/* 18 (SYSTEM) */
			Entity->GroupSid = SidAdmins;
			Entity->Privileges = USER_PRIV_ADMIN;
			Entity->Integrity = SECURITY_MANDATORY_SYSTEM_RID;
			break;
		default:				/* 6 (SERVICE) */
			Entity->GroupSid = SidBatch;
			Entity->Privileges = USER_PRIV_GUEST;
			Entity->Integrity = SECURITY_MANDATORY_LOW_RID;
	}
	return(bResult);
}

/************************************************************/

BOOL 
win_getpwnam(LPCWSTR Account, WIN_PWENT *Result)
{
	BOOL bResult = FALSE;

	ZeroMemory(Result, sizeof(WIN_PWENT));
	if (AclLookupW(Account, &Result->UserSid)){
		win_wcpcpy(Result->Account, win_basename(Account));
		bResult = PwdLookup(Result);
	}
	return(bResult);
}

BOOL 
win_getpwuid(SID8 *Sid, WIN_PWENT *Result)
{
	DWORD accLen = MAX_NAME;
	DWORD domLen = MAX_NAME;
	SID_NAME_USE snType = 0;
	BOOL bResult = FALSE;

	ZeroMemory(Result, sizeof(WIN_PWENT));
	if (LookupAccountSidW(NULL, Sid, Result->Account, &accLen, Result->Domain, &domLen, &snType)){
		CopySid(GetLengthSid(Sid), &Result->UserSid, Sid);
		bResult = PwdLookup(Result);
	}
	return(bResult);
}
BOOL 
win_setpwent(WIN_PWENUM *Enum, DWORD Type)
{
	BOOL bResult = TRUE;
	NET_API_STATUS naStatus;

	if (!Enum->Data){
		naStatus = NetUserEnum(NULL, 0, FILTER_NORMAL_ACCOUNT, &Enum->Data, MAX_PREFERRED_LENGTH, &Enum->Count, &Enum->Total, &Enum->Resume);
		if (naStatus != NERR_Success){
			SetLastError(naStatus);
			bResult = FALSE;
		}
	}
	Enum->Index = 0;
	return(bResult);
}
BOOL 
win_getpwent(WIN_PWENUM *Enum, WIN_PWENT *Result)
{
	BOOL bResult = FALSE;
	DWORD dwIndex = Enum->Index;
	LPUSER_INFO_0 puInfo = (LPUSER_INFO_0)Enum->Data;

	if (!puInfo){
		SetLastError(ERROR_INVALID_USER_BUFFER);
	}else if (dwIndex < Enum->Count){
		win_getpwnam(puInfo[dwIndex].usri0_name, Result);
		Enum->Index++;
		bResult = TRUE;
	}else{
		SetLastError(ERROR_NO_MORE_ITEMS);
	}
	return(bResult);
}
VOID 
win_endpwent(WIN_PWENUM *Info)
{
	NetApiBufferFree(Info->Data);
	Info->Data = NULL;
	Info->Index = 0;
	Info->Count = 0;
	Info->Resume = 0;
}
