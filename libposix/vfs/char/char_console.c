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

BOOL CALLBACK 
ConControlHandler(DWORD CtrlType)
{
	BOOL bResult = TRUE;
	DWORD dwGroupId = __CTTY->GroupId;
	DWORD dwMode = InputMode(&__CTTY->Mode);
	WIN_TASK *pwTask = &__Tasks[__TaskId];

	/* To deliver signals, Windows (CSRSS.EXE) actually forks!
	 * Copying the call stack to a new thread and executing
	 * our code. Let's make sure it uses our Task struct too:
	 */
	TlsSetValue(__TlsIndex, (PVOID)__TaskId);
	if (dwMode & ENABLE_PROCESSED_INPUT){
		if (pwTask->GroupId == dwGroupId){
			if (!vfs_raise(WM_COMMAND, CtrlType, 0)){
				pwTask->Flags |= WIN_PS_EXITING;
				/* causes ExitProcess() */
				bResult = FALSE;
			}else{
				/* syslogd.exe -d */
				vfs_kill_ANY(pwTask->TaskId, WM_COMMAND, CtrlType, 0);
				SetEvent(__Interrupt);
			}
		}
	}
	return(bResult);
}

/****************************************************/

BOOL 
con_TIOCGWINSZ(WIN_DEVICE *Device, WIN_WINSIZE *WinSize)
{
	return(screen_TIOCGWINSZ(Device->Output, WinSize));
}
BOOL 
con_TIOCSWINSZ(WIN_DEVICE *Device, WIN_WINSIZE *WinSize)
{
	return(screen_TIOCSWINSZ(Device->Output, WinSize));
}
BOOL 
con_TIOCSETA(WIN_DEVICE *Device, WIN_TERMIO *Mode)
{
	BOOL bResult = FALSE;

	if (!SetConsoleMode(Device->Input, InputMode(Mode))){
		WIN_ERR("SetConsoleMode(%d): %s\n", Device->Input, win_strerror(GetLastError()));
	}else if (!SetConsoleMode(Device->Output, ScreenMode(Mode))){
		WIN_ERR("SetConsoleMode(%d): %s\n", Device->Output, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}

/****************************************************/

VOID 
con_init(WIN_DEVICE *Device)
{
	WIN_FLAGS wFlags = {GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0};
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
	WIN_TERMIO ioMode;

	if (tty_attach(Device)){
		Device->Input = CharOpenFile("CONIN$", &wFlags, &sa);
		Device->Output = CharOpenFile("CONOUT$", &wFlags, &sa);
	}
}
DWORD 
con_poll(WIN_DEVICE *Device, WIN_POLLFD *Info)
{
	DWORD dwResult = 0;

	dwResult += input_poll(Device->Input, Info);
	dwResult += screen_poll(Device->Output, Info);
	return(dwResult);
}
