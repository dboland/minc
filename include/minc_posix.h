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

#include "minc_types.h"

/* errno.c */

DWORD errno_win(VOID);
int errno_posix(DWORD Error);

/* proc.c */

void task_init(char *cmdbuf, char *argv[], void *frame_address);

/* stat.c */

int rid_posix(SID8 *Sid);
SID8 *rid_win(SID8 *Buf, int rid);

/* namei.c */

char *path_posix(char *dest, LPCWSTR Source);
char *pathp_posix(char *dest, LPCWSTR Source);
WIN_NAMEIDATA *path_win(WIN_NAMEIDATA *Path, const char *pathname, int flags);

/* unistd.c */

int group_member(gid_t gid);

/* netdb.c */

int h_errno_posix(VOID);

/* compat.c */

int runcmd(char *argv[]);

/* grp.c */

char *group_posix(char *buf, size_t buflen, WIN_GRENT *Group);

/* pwd.c */

char *passwd_posix(char *buf, size_t buflen, WIN_PWENT *WinPwd);
