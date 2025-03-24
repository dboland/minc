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

HANDLE 
EventCreateFile(LPCSTR Name, BOOL Manual)
{
	HANDLE hResult = NULL;
	WIN_OBJECT_CONTROL wControl;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), &wControl.Security, FALSE};

	if (!AclCreateControl(EVENT_ALL_ACCESS, EVENT_ALL_ACCESS, &wControl)){
		return(NULL);
	}else if (!(hResult = CreateEvent(&sa, Manual, FALSE, Name))){
		WIN_ERR("CreateEvent(%s): %s\n", Name, win_strerror(GetLastError()));
	}
	return(hResult);
}

/************************************************************/

HANDLE 
event_attach(LPCSTR Name, BOOL Manual)
{
	HANDLE hResult = NULL;

	if (!(hResult = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, Name))){
		hResult = EventCreateFile(Name, Manual);
	}
	return(hResult);
}
VOID 
event_detach(HANDLE Handle)
{
	if (!SetEvent(Handle)){
		WIN_ERR("SetEvent(%d): %s\n", Handle, win_strerror(GetLastError()));
	}
	CloseHandle(Handle);
}
