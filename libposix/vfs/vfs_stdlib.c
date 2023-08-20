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

BOOL 
vfs__cxa_finalize(PVOID Param)
{
	DWORD dwIndex = WIN_ATEXIT_MAX;
	WIN_TASK *pwTask = &__Tasks[CURRENT];
	WIN_ATEXITPROC Function;

	while (dwIndex--){
		if (Function = pwTask->AtExit[dwIndex]){
//__PRINTF("vfs__cxa_finalize(%d): Function(0x%x)\n", dwIndex, Function);
			Function();
		}
	}
}
BOOL 
vfs_realpath(WIN_NAMEI *Path, DWORD Size)
{
	BOOL bResult = FALSE;
	LPWSTR pszDoc = NULL;
	WCHAR szRealPath[WIN_PATH_MAX] = L"";

	/* GetFullPathName() does not merge slashes */
	if (!GetFullPathNameW(Path->Resolved, Size, szRealPath, &pszDoc)){	/* building perl.exe */
		WIN_ERR("GetFullPathName(%ls): %s\n", Path->Resolved, win_strerror(GetLastError()));
	}else{
		Path->R = win_wcpcpy(Path->Resolved, szRealPath);
		Path->Last = Path->R - 1;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_atexit(WIN_ATEXITPROC Function)
{
	DWORD dwIndex = 0;
	WIN_TASK *pwTask = &__Tasks[CURRENT];

	while (dwIndex < WIN_ATEXIT_MAX){
		if (!pwTask->AtExit[dwIndex]){
			pwTask->AtExit[dwIndex] = Function;
			return(TRUE);
		}
		dwIndex++;
	}
	SetLastError(ERROR_NOT_ENOUGH_MEMORY);
	return(FALSE);
}
