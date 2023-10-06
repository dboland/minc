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
#include "arch_posix.h"

#include <limits.h>
#include <string.h>
#include <errno.h>
#include <grp.h>

#define GRPBUF_MAX	MAX_TEXT

char			*_MEMBUF[NGROUPS_MAX];

struct group		_GROUP;
char			_GRPBUF[GRPBUF_MAX];

WIN_PWENUM		_GRPENUM;

/************************************************************/

char *
group_posix(char *buf, size_t buflen, WIN_GRENT *Group)
{
	char *result = buf;

	bzero(buf, buflen);
	buf = win_wcstombp(buf, Group->Account, MAX_NAME);
	buf = stpcpy(buf, ":*:");
//	buf = stpcpy(buf, Group->Comment);
//	buf = stpcpy(buf, ":");
	buf += sprintf(buf, "%lu", rid_posix(&Group->Sid));
	buf = stpcpy(buf, ":");
	buf = stpcpy(buf, Group->Members);
	return(result);
}
char *
grp_skip(char *p, char c)
{
	while (*p && *p != c){
		p++;
	}
	*p++ = 0;
	return(p);
}
struct group *
grent_posix(struct group *grp, char *buf)
{
	int count = 0;

	grp->gr_name = buf;
	buf = grp_skip(buf, ':');
	grp->gr_passwd = buf;
//	buf = grp_skip(buf, ':');		/* comment (gecos) */
	buf = grp_skip(buf, ':');		/* gid */
	grp->gr_gid = strtoul(buf, &buf, 10);
	if (grp->gr_gid == WIN_ROOT_GID){
		grp->gr_gid = 0;
	}
	*buf++ = 0;			/* members */
	while (*buf){
		_MEMBUF[count++] = buf;
		buf = grp_skip(buf, ',');
	}
	_MEMBUF[count] = NULL;
	grp->gr_mem = _MEMBUF;
	return(grp);
}

/************************************************************/

int 
setgroupent(int stayopen)
{
	int result = -1;

	if (!win_setgrent(&_GRPENUM, WIN_NETENUM_LOCAL)){
		errno = errno_posix(errno_win());
	}else{
		result = 0;
	}
	return(result);
}
void 
setgrent(void)
{
	setgroupent(0);
}
void 
endgrent(void)
{
	if (_GRPENUM.Data){
		win_endgrent(&_GRPENUM);
	}
}
int 
getgrent_r(struct group *grp, char *buf, size_t buflen, struct group **result)
{
	int status = -1;
	WIN_GRENT wgResult;

	if (!_GRPENUM.Data){
		setgroupent(1);
	}
	if (!win_getgrent(&_GRPENUM, &wgResult)){
		errno = errno_posix(errno_win());
	}else{
		*result = grent_posix(grp, group_posix(buf, buflen, &wgResult));
		status = 0;
	}
	return(status);
}
int 
getgrnam_r(const char *name, struct group *grp, char *buf, size_t buflen, struct group **result)
{
	int status = -1;
	WCHAR szAccount[MAX_NAME];
	WIN_GRENT wgResult;

	if (!name){
		errno = EINVAL;
	}else if (!buf || !result){
		errno = EFAULT;
	}else if (!win_mbstowcs(szAccount, name, MAX_NAME)){
		errno = EINVAL;
	}else if (!win_getgrnam(szAccount, &wgResult)){
		errno = errno_posix(errno_win());
	}else{
		*result = grent_posix(grp, group_posix(buf, buflen, &wgResult));
		status = 0;
	}
	return(status);
}
int 
getgrgid_r(gid_t gid, struct group *grp, char *buf, size_t buflen, struct group **result)
{
	int status = -1;
	WIN_GRENT wgResult;
	SID8 sid;

	if (!gid){
		gid = WIN_ROOT_GID;
	}
	if (gid < 0){
		errno = EINVAL;
	}else if (!buf || !result){
		errno = EFAULT;
	}else if (!win_getgrgid(rid_win(&sid, gid), &wgResult)){
		errno = errno_posix(errno_win());
	}else{
		*result = grent_posix(grp, group_posix(buf, buflen, &wgResult));
		status = 0;
	}
	return(status);
}
struct group *
getgrnam(const char *name)
{
	struct group *result;

	if (!getgrnam_r(name, &_GROUP, _GRPBUF, GRPBUF_MAX, &result)){
		return(result);
	}
	return(NULL);
}
struct group *
getgrgid(gid_t gid)
{
	struct group *result;

	if (!getgrgid_r(gid, &_GROUP, _GRPBUF, GRPBUF_MAX, &result)){
		return(result);
	}
	return(NULL);
}
struct group *
getgrent(void)
{
	struct group *result;

	if (!getgrent_r(&_GROUP, _GRPBUF, GRPBUF_MAX, &result)){
		return(result);
	}
	return(NULL);
}
