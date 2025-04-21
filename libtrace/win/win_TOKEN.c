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

#include "config.h"

DWORD 
win_TOKEN(HANDLE Token, LPSTR Buffer)
{
	LPSTR psz = Buffer;
	TOKEN_TYPE Type;
	DWORD dwSize;
	LONG lSize = 0;
	BOOL bRestricted = IsTokenRestricted(Token);
	LPSTR pszName = "restricted";
	DWORD dwSessionId = 0;
	TOKEN_STATISTICS tStats = {0};
	TOKEN_SOURCE tSource = {0};
	TOKEN_USER *ptUser;
	TOKEN_PRIMARY_GROUP *ptPrimary;
	TOKEN_OWNER *ptOwner;
	TOKEN_GROUPS *ptGroups;
	TOKEN_PRIVILEGES *ptPrivileges;
	TOKEN_DEFAULT_DACL *ptDefault;

	if (!GetTokenInformation(Token, TokenType, &Type, sizeof(TOKEN_TYPE), &dwSize)){
		WIN_ERR("GetTokenInformation(TokenType): %s\n", win_strerror(GetLastError()));
	}
	if (!bRestricted){
		pszName = "unrestricted";
	}
	if (Type == TokenPrimary){
		psz += sprintf(psz, "Type: primary (%s)\n", pszName);
	}else{
		psz += sprintf(psz, "Type: impersonation (%s)\n", pszName);
	}
	if (!GetTokenInformation(Token, TokenStatistics, &tStats, sizeof(TOKEN_STATISTICS), &dwSize)){
		WIN_ERR("GetTokenInformation(TokenStatistics): %s\n", win_strerror(GetLastError()));
	}
	psz += sprintf(psz, "TokenId: 0x%lx\n", tStats.TokenId.LowPart);
	psz += sprintf(psz, "AuthenticationId: 0x%lx\n", tStats.AuthenticationId.LowPart);
	if (!GetTokenInformation(Token, TokenSessionId, &dwSessionId, sizeof(DWORD), &dwSize)){
		WIN_ERR("GetTokenInformation(TokenSessionId): %s\n", win_strerror(GetLastError()));
	}
	psz += sprintf(psz, "TokenSessionId: 0x%lx\n", dwSessionId);
	if (!GetTokenInformation(Token, TokenSource, &tSource, sizeof(TOKEN_SOURCE), &dwSize)){
		WIN_ERR("GetTokenInformation(TokenSource): %s\n", win_strerror(GetLastError()));
	}
	psz += sprintf(psz, "TokenSource: %s (0x%x)\n", tSource.SourceName, tSource.SourceIdentifier.LowPart);
	lSize = 0;
	GetTokenInformation(Token, TokenOwner, NULL, 0, &lSize);
	if (lSize > 0){
		ptOwner = LocalAlloc(LMEM_FIXED, lSize);
		GetTokenInformation(Token, TokenOwner, ptOwner, lSize, &dwSize);
		psz = win_SID(psz, "TokenOwner", ptOwner->Owner);
		LocalFree(ptOwner);
	}else{
		WIN_ERR("GetTokenInformation(TokenOwner): %s\n", win_strerror(GetLastError()));
	}
	lSize = 0;
	GetTokenInformation(Token, TokenUser, NULL, 0, &lSize);
	if (lSize > 0){
		ptUser = LocalAlloc(LMEM_FIXED, lSize);
		GetTokenInformation(Token, TokenUser, ptUser, lSize, &dwSize);
		psz = win_SID(psz, "TokenUser", ptUser->User.Sid);
		psz = WinGroupAttribs(psz, "  Attributes", ptUser->User.Attributes);
		LocalFree(ptUser);
	}else{
		WIN_ERR("GetTokenInformation(TokenUser): %s\n", win_strerror(GetLastError()));
	}
	lSize = 0;
	GetTokenInformation(Token, TokenPrimaryGroup, NULL, 0, &lSize);
	if (lSize > 0){
		ptPrimary = LocalAlloc(LMEM_FIXED, lSize);
		GetTokenInformation(Token, TokenPrimaryGroup, ptPrimary, lSize, &dwSize);
		psz = win_SID(psz, "TokenPrimaryGroup", ptPrimary->PrimaryGroup);
		LocalFree(ptPrimary);
	}else{
		WIN_ERR("GetTokenInformation(TokenPrimaryGroup): %s\n", win_strerror(GetLastError()));
	}
	lSize = 0;
	GetTokenInformation(Token, TokenGroups, NULL, 0, &lSize);
	if (lSize > 0){
		ptGroups = LocalAlloc(LMEM_FIXED, lSize);
		GetTokenInformation(Token, TokenGroups, ptGroups, lSize, &dwSize);
		psz = win_TOKEN_GROUPS(psz, "TokenGroups", ptGroups);
		LocalFree(ptGroups);
	}else{
		WIN_ERR("GetTokenInformation(TokenGroups): %s\n", win_strerror(GetLastError()));
	}
	lSize = 0;
	GetTokenInformation(Token, TokenDefaultDacl, NULL, 0, &lSize);
	if (lSize > 0){
		ptDefault = LocalAlloc(LMEM_FIXED, lSize);
		GetTokenInformation(Token, TokenDefaultDacl, ptDefault, lSize, &dwSize);
		psz = win_ACL(psz, "TokenDefaultDacl", ptDefault->DefaultDacl, OB_TYPE_PROCESS);
		LocalFree(ptDefault);
	}else{
		WIN_ERR("GetTokenInformation(TokenDefaultDacl): %s\n", win_strerror(GetLastError()));
	}
	lSize = 0;
	GetTokenInformation(Token, TokenPrivileges, NULL, 0, &lSize);
	if (lSize > 0){
		ptPrivileges = LocalAlloc(LMEM_FIXED, lSize);
		GetTokenInformation(Token, TokenPrivileges, ptPrivileges, lSize, &dwSize);
		psz = win_TOKEN_PRIVILEGES(psz, "TokenPrivileges", ptPrivileges);
		LocalFree(ptPrivileges);
	}else{
		WIN_ERR("GetTokenInformation(TokenPrivileges): %s\n", win_strerror(GetLastError()));
	}
	return(psz - Buffer);
}
