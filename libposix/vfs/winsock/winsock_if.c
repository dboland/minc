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

#include <iphlpapi.h>

/****************************************************/

UINT 
ws2_nametoindex(LPCSTR Name)
{
	WIN_DEVICE *pwDevice = DEVICE(DEV_CLASS_IFNET);
	USHORT sUnit = 0;
	UINT uiError = ERROR_BAD_DEVICE;

	while (sUnit < WIN_UNIT_MAX){
		if (!strcmp(pwDevice->Name, Name)){
			if (!pwDevice->Flags){
				uiError = ERROR_DEVICE_NOT_AVAILABLE;
				break;
			}else{
				return(pwDevice->Index);
			}
		}
		pwDevice++;
		sUnit++;
	}
	SetLastError(uiError);
	return(0);
}
UINT 
ws2_indextoname(DWORD Index, LPSTR Result)
{
	WIN_DEVICE *pwDevice = DEVICE(DEV_CLASS_IFNET);
	USHORT sUnit = 0;
	UINT uiError = ERROR_BAD_DEVICE;

	if (!Index){		/* XP SP2 */
		uiError = ERROR_INVALID_PARAMETER;
	}else while (sUnit < WIN_UNIT_MAX){
		if (pwDevice->Index == Index){
			win_strcpy(Result, pwDevice->Name);
			return(strlen(Result));
		}
		pwDevice++;
		sUnit++;
	}
	SetLastError(uiError);
	return(0);
}
