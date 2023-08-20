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

#include <sys/mman.h>

/****************************************************/

void *
sys_mmap(call_t call, void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	DWORD dwProtect = PAGE_READONLY;
	void *result = (void *)-1;
	WIN_TASK *pwTask = call.Task;

	if (prot & PROT_WRITE){
		dwProtect = PAGE_READWRITE;
	}
	if (flags & MAP_ANON){
		fd = -1;
	}
	if (!length){
		__errno_posix(pwTask, ERROR_BAD_ARGUMENTS);
	}else if (!win_mmap(fd_win(pwTask, fd), offset, length, addr, dwProtect, &result)){
		__errno_posix(pwTask, GetLastError());
	}
	return(result);
}
int 
sys_munmap(call_t call, void *addr, size_t length)
{
	int result = -1;

	if (!win_munmap(addr, length)){
		__errno_posix(call.Task, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
int 
sys_madvise(call_t call, void *addr, size_t length, int advice)
{
	/* install.exe (MADV_SEQUENTIAL) */
//	msvc_printf("sys_madvise: %d\n", advice);
	return(0);
}
int 
sys_mprotect(call_t call, void *addr, size_t len, int prot)
{
	DWORD dwProtect = PAGE_READONLY;
	int result = -1;

	if (prot & PROT_WRITE){
		dwProtect = PAGE_READWRITE;
	}
	if (!len){
		__errno_posix(call.Task, ERROR_BAD_ARGUMENTS);
	}else if (!win_mprotect(addr, len, dwProtect)){
		__errno_posix(call.Task, GetLastError());
	}else{
		result = 0;
	}
	return(result);
}
