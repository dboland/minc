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

#include <dlfcn.h>

char _DL_PATH_BUF[PATH_MAX];

/**************************************************************/

void *
dlopen(const char *filename, int flag)
{
	void *result = NULL;
	WIN_NAMEIDATA wPath;
	WIN_TASK *pwTask = &__Tasks[CURRENT];

	if (!filename){
		result = win_dlopen(NULL);
	}else if (!(result = win_dlopen(path_win(&wPath, filename, 0)->Resolved))){
		pwTask->Error = errno_posix(GetLastError());
	}
	return(result);
}
void *
dlsym(void *handle, const char *symbol)
{
	void *result = NULL;
	WIN_TASK *pwTask = &__Tasks[CURRENT];

	if (!(result = win_dlsym(handle, symbol))){
		pwTask->Error = errno_posix(GetLastError());
	}
	return(result);
}
char *
dlerror(void)
{
	return(win_strerror(errno_win()));
}
int 
dlclose(void *handle)
{
	return(win_dlclose(handle));
}
int 
dladdr(const void *addr, Dl_info *info)
{
	int result = 0;
	MEMORY_BASIC_INFORMATION mbInfo;
	WCHAR szPath[MAX_PATH];
	WIN_TASK *pwTask = &__Tasks[CURRENT];

	if (!win_dladdr(addr, &mbInfo, szPath)){
		pwTask->Error = errno_posix(GetLastError());
	}else{
		info->dli_fname = path_posix(_DL_PATH_BUF, szPath);
		info->dli_fbase = mbInfo.AllocationBase;
		info->dli_sname = NULL;
		info->dli_saddr = NULL;
		result = 1;
	}
}
