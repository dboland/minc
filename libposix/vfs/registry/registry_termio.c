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

#include <winreg.h>

/****************************************************/

BOOL 
reg_TIOCGETA(WIN_DEVICE *Device, DWORD Mode[2])
{
	BOOL bResult = FALSE;
	DWORD dwValue = 0;
	WIN_VNODE vNode = {0};
	DWORD dwSize = sizeof(DWORD);
	LPCWSTR pszPath = L"Console\\MinC";

	if (!RegOpenFile(HKEY_CURRENT_USER, pszPath, KEY_QUERY_VALUE, &vNode)){
		WIN_ERR("RegOpenFile(%ls): %s\n", pszPath, win_strerror(GetLastError()));
	}else{
		RegReadFile(vNode.Key, L"VirtualTerminalLevel", REG_DWORD, &dwValue, &dwSize);
		if (dwValue > 0){
			Mode[1] |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		}
		if (dwValue == 2){
			Mode[0] |= ENABLE_VIRTUAL_TERMINAL_INPUT;
		}
		RegCloseKey(vNode.Key);
		bResult = TRUE;
	}
	return(bResult);
}
