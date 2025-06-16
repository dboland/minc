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

#include <windows.h>

#include "win_types.h"
#include "vfs_posix.h"

extern WIN_TASK 	*__Tasks;
extern WIN_TASK 	*__Process;
extern DWORD		__TlsIndex;

BOOL posix_PROCESS_DETACH(WIN_TASK *Task);
BOOL posix_THREAD_ATTACH(WIN_TASK *Task);
BOOL posix_THREAD_DETACH(WIN_TASK *Task);

BOOL WINAPI 
DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	BOOL bResult = FALSE;

	switch (fdwReason){
		case DLL_PROCESS_ATTACH:
			bResult = vfs_PROCESS_ATTACH(hinstDLL, lpvReserved);
			break;
		case DLL_PROCESS_DETACH:
			bResult = posix_PROCESS_DETACH(&__Tasks[CURRENT]);
			break;
		case DLL_THREAD_ATTACH:
			bResult = posix_THREAD_ATTACH(__Process);
			break;
		case DLL_THREAD_DETACH:
			bResult = posix_THREAD_DETACH(&__Tasks[CURRENT]);
			break;
	}
	return(bResult);
}
