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

BOOL 
char_TIOCGWINSZ(WIN_VNODE *Node, WIN_WINSIZE *WinSize)
{
	BOOL bResult = FALSE;

	switch (Node->DeviceType){
		case DEV_TYPE_CONSOLE:
		case DEV_TYPE_PTY:
			bResult = con_TIOCGWINSZ(DEVICE(Node->DeviceId), WinSize);
			break;
		case DEV_TYPE_SCREEN:
			bResult = screen_TIOCGWINSZ(Node->Handle, WinSize);
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(bResult);
}
BOOL 
char_TIOCSWINSZ(WIN_VNODE *Node, WIN_WINSIZE *WinSize)
{
	BOOL bResult = FALSE;

	switch (Node->DeviceType){
		case DEV_TYPE_CONSOLE:
		case DEV_TYPE_PTY:
			bResult = con_TIOCSWINSZ(DEVICE(Node->DeviceId), WinSize);
			break;
		case DEV_TYPE_SCREEN:
			bResult = screen_TIOCSWINSZ(Node->Handle, WinSize);
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(bResult);
}
BOOL 
char_TIOCSETA(WIN_VNODE *Node, WIN_TERMIO *Mode)
{
	BOOL bResult = FALSE;

	switch (Node->DeviceType){
		case DEV_TYPE_CONSOLE:
		case DEV_TYPE_PTY:
			bResult = con_TIOCSETA(DEVICE(Node->DeviceId), Mode);
			break;
		case DEV_TYPE_INPUT:
			bResult = SetConsoleMode(Node->Handle, InputMode(Mode));
			break;
		case DEV_TYPE_SCREEN:
			bResult = SetConsoleMode(Node->Handle, ScreenMode(Mode));
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(TRUE);
}
BOOL 
char_TIOCFLUSH(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	switch (Node->DeviceType){
		case DEV_TYPE_INPUT:
			bResult = input_TIOCFLUSH(Node->Handle);
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(bResult);
}
BOOL 
char_TIOCDRAIN(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	switch (Node->DeviceType){
		case DEV_TYPE_SCREEN:
			bResult = screen_TIOCDRAIN(Node->Handle);
			break;
		default:
			SetLastError(ERROR_BAD_DEVICE);
	}
	return(bResult);
}
