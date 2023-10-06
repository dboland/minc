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

#include "win/windows.h"
#include "win_posix.h"
#include "arch_posix.h"

#include <errno.h>

int 
getgrouplist(const char *user, gid_t group, gid_t *groups, int *ngroups)
{
	int result = -1;
	WIN_PWENT pwEntry;
	WCHAR szAccount[MAX_NAME];
	SID8 *sidList = NULL;
	SID8 sid;
	int i = 0;
	gid_t next;
	int count = 0;

	if (!group){
		group = WIN_ROOT_GID;
	}
	if (!user){
		errno = EINVAL;
	}else if (!groups || !ngroups){
		errno = EFAULT;
	}else if (!win_mbstowcs(szAccount, user, MAX_NAME)){
		errno = EINVAL;
	}else if (!win_getpwnam(szAccount, &pwEntry)){
		errno = errno_posix(errno_win());
	}else if (!win_getgrouplist(&pwEntry, rid_win(&sid, group), &sidList, &count)){
		errno = errno_posix(errno_win());
	}else if (*ngroups >= count){
		while (i < count){
			next = rid_posix(&sidList[i]);
			if (next == WIN_ROOT_GID){
				groups[i] = 0;
			}else{
				groups[i] = next;
			}
			i++;
		}
		result = count;
	}
	win_free(sidList);
	*ngroups = count;
	return(result);
}
