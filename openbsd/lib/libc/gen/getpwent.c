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

#include <stdio.h>
#include <stdlib.h>

#include "win/windows.h"
#include "win_posix.h"
#include "minc_posix.h"

#include <string.h>
#include <errno.h>
#include <pwd.h>

#define PWDBUF_MAX	PATH_MAX

char 			_PWDBUF[PWDBUF_MAX];
struct passwd		_PASSWD;

WIN_PWENUM		_PWDENUM;

/************************************************************/

char *
passwd_posix(char *buf, size_t buflen, WIN_PWENT *WinPwd)
{
	char *result = buf;
	char *shell = "/bin/ksh";

	buf = win_wcstombp(buf, WinPwd->Account, MAX_NAME);
	buf = stpcpy(buf, ":");
	buf = stpcpy(buf, WinPwd->Password);
	buf = stpcpy(buf, ":");
	buf += sprintf(buf, "%lu", rid_posix(&WinPwd->UserSid));
	buf = stpcpy(buf, ":");
	buf += sprintf(buf, "%lu", rid_posix(&WinPwd->GroupSid));
	/* login class */
	if (WinPwd->Integrity == SECURITY_MANDATORY_LOW_RID){
		buf = stpcpy(buf, ":daemon");
	}else{
		buf = stpcpy(buf, ":default");
	}
	buf = stpcpy(buf, ":");
	buf += sprintf(buf, "%lu", WinPwd->Change);
	buf = stpcpy(buf, ":");
	buf += sprintf(buf, "%lu", WinPwd->Expire);
	buf = stpcpy(buf, ":");
	buf = stpcpy(buf, WinPwd->Comment);
	buf = stpcpy(buf, ":");
	if (*WinPwd->Home){
		buf = pathp_posix(buf, WinPwd->Home);
	}else if (WinPwd->Integrity == SECURITY_MANDATORY_SYSTEM_RID){
		buf = stpcpy(buf, "/root");
	}else if (WinPwd->Integrity == SECURITY_MANDATORY_LOW_RID){
		buf = stpcpy(buf, "/var/empty");
		shell = "/bin/sh";
	}else{
		buf = win_wcstombp(stpcpy(buf, "/home/"), WinPwd->Account, MAX_NAME);
	}
	buf = stpcpy(buf, ":");
	buf = stpcpy(buf, shell);
	return(result);
}
char *
pwd_skip(char *p)
{
	while (*p != ':'){
		p++;
	}
	*p++ = 0;
	return(p);
}
struct passwd *
pwent_posix(struct passwd *pwd, char *buf)
{
	pwd->pw_name = buf;
	buf = pwd_skip(buf);
	pwd->pw_passwd = buf;
	buf = pwd_skip(buf);
	pwd->pw_uid = strtoul(buf, &buf, 10);
	if (pwd->pw_uid == WIN_ROOT_UID){
		pwd->pw_uid = 0;
	}
	*buf++ = 0;
	pwd->pw_gid = strtoul(buf, &buf, 10);
	if (pwd->pw_gid == WIN_ROOT_GID){
		pwd->pw_gid = 0;
	}
	*buf++ = 0;
	pwd->pw_class = buf;
	buf = pwd_skip(buf);
	pwd->pw_change = strtoul(buf, &buf, 10);
	*buf++ = 0;
	pwd->pw_expire = strtoul(buf, &buf, 10);
	*buf++ = 0;
	pwd->pw_gecos = buf;
	buf = pwd_skip(buf);
	pwd->pw_dir = buf;
	buf = pwd_skip(buf);
	pwd->pw_shell = buf;
	return(pwd);
}

/************************************************************/

int 
setpassent(int stayopen)
{
	int result = -1;

	/* see: ./lib/libc/gen/pwcache.c */

	if (!win_setpwent(&_PWDENUM, WIN_NETENUM_LOCAL)){
		errno = errno_posix(errno_win());
	}else{
		result = 0;
	}
	return(result);
}
void 
setpwent(void)
{
	setpassent(0);
}
void 
endpwent(void)
{
	if (_PWDENUM.Data){
		win_endpwent(&_PWDENUM);
	}
}
int 
getpwent_r(struct passwd *pwd, char *buf, size_t buflen, struct passwd **result)
{
	int status = -1;
	WIN_PWENT pwResult;

	if (!_PWDENUM.Data){
		setpassent(1);
	}
	if (!win_getpwent(&_PWDENUM, &pwResult)){
		errno = errno_posix(errno_win());
	}else{
		*result = pwent_posix(pwd, passwd_posix(buf, buflen, &pwResult));
		status = 0;
	}
	return(status);
}
int 
getpwnam_r(const char *name, struct passwd *pwd, char *buf, size_t buflen, struct passwd **result)
{
	int status = -1;
	WCHAR szAccount[MAX_NAME];
	WIN_PWENT pwResult;

	if (!name){
		errno = EINVAL;
	}else if (!buf || !result){
		errno = EFAULT;
	}else if (!win_mbstowcs(szAccount, name, MAX_NAME)){
		errno = EINVAL;
	}else if (!win_getpwnam(szAccount, &pwResult)){
		errno = errno_posix(errno_win());
	}else{
		*result = pwent_posix(pwd, passwd_posix(buf, buflen, &pwResult));
		status = 0;
	}
	return(status);
}
int 
getpwuid_r(uid_t uid, struct passwd *pwd, char *buf, size_t buflen, struct passwd **result)
{
	int status = -1;
	WIN_PWENT pwResult;
	SID8 sid;

	if (!uid){
		uid = WIN_ROOT_UID;
	}
	if (uid < 0){
		errno = EINVAL;
	}else if (!buf || !result){
		errno = EFAULT;
	}else if (!win_getpwuid(rid_win(&sid, uid), &pwResult)){
		errno = errno_posix(errno_win());
	}else{
		*result = pwent_posix(pwd, passwd_posix(buf, buflen, &pwResult));
		status = 0;
	}
	return(status);
}
struct passwd *
getpwnam(const char *name)
{
	struct passwd *result;

	if (!getpwnam_r(name, &_PASSWD, _PWDBUF, PWDBUF_MAX, &result)){
		return(result);
	}
	return(NULL);
}
struct passwd *
getpwuid(uid_t uid)
{
	struct passwd *result;

	if (!getpwuid_r(uid, &_PASSWD, _PWDBUF, PWDBUF_MAX, &result)){
		return(result);
	}
	return(NULL);
}
struct passwd *
getpwent(void)
{
	struct passwd *result;

	if (!getpwent_r(&_PASSWD, _PWDBUF, PWDBUF_MAX, &result)){
		return(result);
	}
	return(NULL);
}

