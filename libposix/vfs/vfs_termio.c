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
vfs_TIOCGWINSZ(WIN_VNODE *Node, WIN_WINSIZE *WinSize)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_PDO:
			bResult = pdo_TIOCGWINSZ(Node->Device, WinSize);
			break;
		case FS_TYPE_CHAR:			/* stty.exe */
			bResult = char_TIOCGWINSZ(Node, WinSize);
			break;
		case FS_TYPE_MAILSLOT:
			*WinSize = __CTTY->WinSize;
			bResult = TRUE;
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
vfs_TIOCGETA(WIN_VNODE *Node, DWORD Mode[2])
{
	BOOL bResult = FALSE;

//VfsDebugNode(Node, "vfs_TIOCGETA");
	switch (Node->FSType){
		case FS_TYPE_PDO:
			bResult = pdo_TIOCGETA(Node->Device, Mode);
			break;
		case FS_TYPE_CHAR:			/* nano.exe */
			bResult = char_TIOCGETA(Node, Mode);
			break;
		case FS_TYPE_MAILSLOT:
			Mode[0] = __CTTY->Mode[0] & 0xFFFF;
			Mode[1] = __CTTY->Mode[1] & 0xFFFF;
			bResult = TRUE;
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
//VfsDebugIoctl(Mode, "vfs_TIOCGETA");
	return(bResult);
}
BOOL 
vfs_TIOCFLUSH(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_PDO:
			bResult = pdo_TIOCFLUSH(Node->Device);
			break;
		case FS_TYPE_CHAR:
			bResult = char_TIOCFLUSH(Node);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
vfs_TIOCDRAIN(WIN_VNODE *Node)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_PDO:
			bResult = pdo_TIOCDRAIN(Node->Device);
			break;
		case FS_TYPE_CHAR:
			bResult = char_TIOCDRAIN(Node);
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
vfs_TIOCSETAF(WIN_VNODE *Node, DWORD Mode[2], BOOL Flush, BOOL Drain)
{
	BOOL bResult = FALSE;

	if (Drain){
		vfs_TIOCDRAIN(Node);
	}
//VfsDebugIoctl(Mode, "vfs_TIOCSETAF");
	switch (Node->FSType){
		case FS_TYPE_PDO:
			bResult = pdo_TIOCSETA(Node->Device, Mode);
			break;
		case FS_TYPE_CHAR:			/* nano.exe */
			bResult = char_TIOCSETA(Node, Mode);
			break;
		case FS_TYPE_MAILSLOT:
//			__CTTY->Mode[0] = Mode[0];
//			__CTTY->Mode[1] = Mode[1];
			bResult = TRUE;
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	if (Flush){
		vfs_TIOCFLUSH(Node);
	}
	return(bResult);
}
BOOL 
vfs_TIOCSCTTY(WIN_DEVICE *Device, WIN_TASK *Task)
{
	BOOL bResult = FALSE;
	WIN_TERMIO *pTerminal;

	if (Task->Flags & WIN_PS_CONTROLT){
		SetLastError(ERROR_LOGON_SESSION_EXISTS);
	}else{
		pTerminal = TTYCreate(Device);
		pTerminal->SessionId = Task->SessionId;
		pTerminal->GroupId = Task->GroupId;
//VfsDebugTTY(pTerminal, "vfs_TIOCSCTTY");
		Task->Flags |= WIN_PS_CONTROLT;
		Task->TerminalId = pTerminal->TerminalId;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_PTMGET(WIN_DEVICE *Device, WIN_PTMGET *Result)
{
	BOOL bResult = FALSE;
	WIN_FLAGS wFlags = {GENERIC_READ + GENERIC_WRITE, 
		FILE_SHARE_READ + FILE_SHARE_WRITE, OPEN_EXISTING, 0, 0};

	if (!MailCreateOutput(Device, &wFlags, &Result->Master)){
		return(FALSE);
	}else if (MailCreateInput(pdo_attach(DEV_TYPE_TTY), &wFlags, &Result->Slave)){
		bResult = mail_PTMGET(Device, Result->Slave.Device);
	}
	return(bResult);
}
BOOL 
vfs_TIOCSFLAGS(WIN_DEVICE *Device, DWORD Param)
{
	msvc_printf("WIN_TIOCSFLAGS(%s): 0x%x\n", Device->Name, Param);
}
BOOL 
vfs_TIOCTL(WIN_VNODE *Node, DWORD Operation, PVOID Param)
{
	BOOL bResult = TRUE;

	switch (Operation){
		case WIN_TIOCFLUSH:
			bResult = vfs_TIOCFLUSH(Node);
			break;
		case WIN_TIOCDRAIN:
			bResult = vfs_TIOCDRAIN(Node);
			break;
		case WIN_TIOCSETA:
			bResult = vfs_TIOCSETAF(Node, (DWORD *)Param, FALSE, FALSE);
			break;
		case WIN_TIOCSETAW:
			bResult = vfs_TIOCSETAF(Node, (DWORD *)Param, FALSE, TRUE);
			break;
		case WIN_TIOCSETAF:
			bResult = vfs_TIOCSETAF(Node, (DWORD *)Param, TRUE, TRUE);
			break;
		case WIN_TIOCGWINSZ:
			bResult = vfs_TIOCGWINSZ(Node, (WIN_WINSIZE *)Param);
			break;
		case WIN_TIOCSWINSZ:
			__CTTY->WinSize = *(WIN_WINSIZE *)Param;
			break;
		case WIN_TIOCSPGRP:
			__CTTY->GroupId = *(UINT *)Param;
			break;
		case WIN_TIOCGPGRP:
			*(UINT *)Param = __CTTY->GroupId;
			break;
		/* line discipline: raw/cooked mode, see ./sys/kern/tty_conf.c */
		case WIN_TIOCGETD:
			*(UINT *)Param = (__CTTY->Mode[0] & ENABLE_LINE_INPUT);
			break;
		case WIN_TIOCSFLAGS:		/* ttyflags.exe */
			bResult = vfs_TIOCSFLAGS(DEVICE(Node->DeviceId), *(DWORD *)Param);
			break;
		case WIN_TIOCGFLAGS:
			*(DWORD *)Param = __CTTY->Flags;
			break;
		default:
			SetLastError(ERROR_NOT_SUPPORTED);
			bResult = FALSE;
	}
	return(bResult);
}
