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
vfs_TIOCSWINSZ(WIN_VNODE *Node, WIN_WINSIZE *WinSize)
{
	BOOL bResult = FALSE;

	switch (Node->FSType){
		case FS_TYPE_PDO:
			bResult = pdo_TIOCSWINSZ(Node->Device, WinSize);
			break;
		case FS_TYPE_CHAR:
			bResult = char_TIOCSWINSZ(Node, WinSize);
			break;
		case FS_TYPE_MAILSLOT:
			__CTTY->WinSize = *WinSize;
			bResult = TRUE;
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
vfs_TIOCGETA(WIN_VNODE *Node, WIN_TERMIO *Mode)
{
	BOOL bResult = FALSE;

	switch (Node->DeviceType){
		case DEV_TYPE_PTY:
		case DEV_TYPE_CONSOLE:
		case DEV_TYPE_TTY:
		case DEV_CLASS_TTY:
		case DEV_TYPE_INPUT:
		case DEV_TYPE_SCREEN:
		case DEV_TYPE_COM:
			bResult = TRUE;
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
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
vfs_TIOCSETA(WIN_VNODE *Node, WIN_TERMIO *Mode, BOOL Flush, BOOL Drain)
{
	BOOL bResult = FALSE;

	if (Drain){
		vfs_TIOCDRAIN(Node);
	}
	switch (Node->FSType){
		case FS_TYPE_PDO:
			bResult = pdo_TIOCSETA(Node->Device, Mode);
			break;
		case FS_TYPE_CHAR:			/* nano.exe */
			bResult = char_TIOCSETA(Node, Mode);
			break;
		case FS_TYPE_MAILSLOT:
//vfs_ktrace("vfs_TIOCSETA", STRUCT_VNODE, Node);
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
vfs_TIOCSCTTY(WIN_TTY *Terminal, WIN_TASK *Task)
{
	BOOL bResult = FALSE;

	if (Task->Flags & WIN_PS_CONTROLT){
		SetLastError(ERROR_LOGON_SESSION_EXISTS);
	}else{
		Terminal->SessionId = Task->SessionId;
		Terminal->GroupId = Task->GroupId;
		Task->Flags |= WIN_PS_CONTROLT;
		Task->CTTY = Terminal->Index;
		bResult = TRUE;
	}
//vfs_ktrace("vfs_TIOCSCTTY", STRUCT_TTY, Terminal);
	return(bResult);
}
BOOL 
vfs_PTMGET(WIN_VNODE *Master, WIN_VNODE *Slave)
{
	BOOL bResult = FALSE;
//	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};

	if (MailCreateSlave(pdo_attach(DEV_TYPE_TTY), __MailEvent, &sa, Slave)){
		Master->Event = __MailEvent;
		Slave->Index = Master->Index;
		bResult = pdo_PTMGET(Master->Device, Slave->Device);
	}
	return(bResult);
}
BOOL 
vfs_TIOCGPGRP(WIN_TTY *Terminal, UINT *Result)
{
	BOOL bResult = FALSE;

	if (!Terminal->Flags){
		SetLastError(ERROR_CTX_NOT_CONSOLE);
	}else{
		*Result = Terminal->GroupId;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_TIOCSPGRP(WIN_TTY *Terminal, UINT GroupId)
{
	BOOL bResult = FALSE;

	if (!Terminal->Flags){
		SetLastError(ERROR_CTX_NOT_CONSOLE);
	}else{
		Terminal->GroupId = GroupId;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_TIOCGFLAGS(WIN_TTY *Terminal, UINT *Result)
{
	BOOL bResult = FALSE;

	if (!Terminal->Flags){
		SetLastError(ERROR_CTX_NOT_CONSOLE);
	}else{
		*Result = Terminal->Flags & 0xFFFF;
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_TIOCSFLAGS(WIN_TTY *Terminal, UINT Flags)
{
	BOOL bResult = FALSE;
	DWORD dwFlags = Terminal->Flags & 0xFFFF0000;

	if (!Terminal->Flags){
		SetLastError(ERROR_CTX_NOT_CONSOLE);
	}else{
		Terminal->Flags = dwFlags | Flags;
		bResult = TRUE;
	}
	return(bResult);
}
