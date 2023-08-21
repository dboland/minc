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

/****************************************************/

WIN_TERMIO *
TTYCreate(WIN_DEVICE *Device)
{
	DWORD dwIndex = 1;
	WIN_TERMIO *ptResult = &__Terminals[dwIndex];

	while (dwIndex < WIN_TTY_MAX){
		if (!ptResult->TerminalId){
			ptResult->TerminalId = dwIndex;
			ptResult->DeviceId = Device->DeviceId;
			if (!dev_TIOCGETA(Device, ptResult->Mode)){
				ptResult->Mode[0] = ENABLE_PROCESSED_INPUT + ENABLE_ECHO_INPUT + ENABLE_LINE_INPUT;
				ptResult->Mode[1] = ENABLE_PROCESSED_OUTPUT + ENABLE_WRAP_AT_EOL_OUTPUT;
			}
			if (!dev_TIOCGWINSZ(Device, &ptResult->WinSize)){
				ptResult->WinSize.Row = 30;
				ptResult->WinSize.Column = 80;
			}
//			reg_TIOCGETA(Device, ptResult->Mode);
			/* CR is MSDOS default, but UNIX cooked mode expects NL */
			ptResult->Mode[0] |= WIN_ICRNL | ENABLE_WINDOW_INPUT;
			ptResult->Mode[1] |= WIN_ONLCR | WIN_OXTABS;
			ptResult->ScrollRate = 1;
			return(ptResult);
		}
		ptResult++;
		dwIndex++;
	}
	SetLastError(ERROR_NOT_ENOUGH_MEMORY);
	return(NULL);
}

/****************************************************/

BOOL 
tty_open(WIN_DEVICE *Device, WIN_FLAGS *Flags, WIN_VNODE *Result)
{
	Result->Event = Device->Input;
	Result->Device = Device;
	return(dev_activate(Device, Result));
}
BOOL 
tty_close(WIN_TERMIO *Terminal)
{
	BOOL bResult = FALSE;

	if (dev_revoke(DEVICE(Terminal->DeviceId))){
		ZeroMemory(Terminal, sizeof(WIN_TERMIO));
		bResult = TRUE;
	}
	return(bResult);
}
