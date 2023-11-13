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

#include <wincon.h>

/****************************************************/

BOOL 
pty_open(WIN_DEVICE *Device, WIN_FLAGS *Flags, WIN_VNODE *Result)
{
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};

	Device->Input = raw_open("CONIN$", Flags, &sa);
	Device->Output = raw_open("CONOUT$", Flags, &sa);
	Result->Event = Device->Input;
	Result->Device = Device;
	return(config_activate(Device, Result));
}
BOOL 
pty_revoke(WIN_DEVICE *Device)
{
	BOOL bResult = FALSE;

	if (!CloseHandle(Device->Input)){
		WIN_ERR("CloseHandle(%d): %s\n", Device->Input, win_strerror(GetLastError()));
	}else if (!CloseHandle(Device->Output)){
		WIN_ERR("CloseHandle(%d): %s\n", Device->Output, win_strerror(GetLastError()));
	}else if (!CloseHandle(Device->Handle)){
		WIN_ERR("CloseHandle(%d): %s\n", Device->Handle, win_strerror(GetLastError()));
	}else{
		ZeroMemory(Device, sizeof(WIN_DEVICE));
		bResult = TRUE;
	}
	return(bResult);
}
