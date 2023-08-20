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

#include <winbase.h>

/************************************************************/

DECLSPEC_NORETURN VOID 
win_exit(DWORD ExitCode)
{
	if (__ThreadCount--){
		ExitThread(ExitCode);
	}else{
		ExitProcess(ExitCode);
	}
}
BOOL 
win_execve(LPSTR Command, LPCSTR Path)
{
	BOOL bResult = FALSE;
	PROCESS_INFORMATION pi = {0};
	STARTUPINFO si = {0};

	win_F_SETFD(GetStdHandle(STD_INPUT_HANDLE), HANDLE_FLAG_INHERIT);
	win_F_SETFD(GetStdHandle(STD_OUTPUT_HANDLE), HANDLE_FLAG_INHERIT);
	win_F_SETFD(GetStdHandle(STD_ERROR_HANDLE), HANDLE_FLAG_INHERIT);
	si.cb = sizeof(STARTUPINFO);
	si.lpDesktop = "";			/* Vista */
	if (CreateProcess(NULL, Command, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, Path, &si, &pi)){
		CloseHandle(pi.hThread);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		bResult = TRUE;
	}
	win_free(Command);
	return(bResult);
}
