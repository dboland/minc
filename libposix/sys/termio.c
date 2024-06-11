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

#include <sys/termios.h>

/****************************************************/

void 
termio_debug(WIN_TERMIO *Mode, const char *lable)
{
	DWORD dwRemain;

	msvc_printf("%s:\n", lable);

	dwRemain = Mode->LFlags;
	msvc_printf("+ local([0x%x]", dwRemain);
	win_flagname(ECHOKE, "ECHOKE", dwRemain, &dwRemain);
	win_flagname(ECHOE, "ECHOE", dwRemain, &dwRemain);
	win_flagname(ECHO, "ECHO", dwRemain, &dwRemain);
	win_flagname(ECHOCTL, "ECHOCTL", dwRemain, &dwRemain);
	win_flagname(ISIG, "ISIG", dwRemain, &dwRemain);
	win_flagname(ICANON, "ICANON", dwRemain, &dwRemain);
	win_flagname(IEXTEN, "IEXTEN", dwRemain, &dwRemain);
	win_flagname(NOKERNINFO, "NOKERNINFO", dwRemain, &dwRemain);
	msvc_printf("[0x%x])\n", dwRemain);

	dwRemain = Mode->IFlags;
	msvc_printf("+ input([0x%x]", dwRemain);
	win_flagname(INLCR, "INLCR", dwRemain, &dwRemain);
	win_flagname(ICRNL, "ICRNL", dwRemain, &dwRemain);
	win_flagname(IXANY, "IXANY", dwRemain, &dwRemain);
	win_flagname(IXON, "IXON", dwRemain, &dwRemain);
	win_flagname(PARMRK, "PARMRK", dwRemain, &dwRemain);
	win_flagname(BRKINT, "BRKINT", dwRemain, &dwRemain);
	win_flagname(IGNPAR, "IGNPAR", dwRemain, &dwRemain);
	win_flagname(IMAXBEL, "IMAXBEL", dwRemain, &dwRemain);
	msvc_printf("[0x%x])\n", dwRemain);

	dwRemain = Mode->OFlags;
	msvc_printf("+ output([0x%x]", dwRemain);
	win_flagname(OPOST, "OPOST", dwRemain, &dwRemain);
	win_flagname(OXTABS, "OXTABS", dwRemain, &dwRemain);
	win_flagname(OCRNL, "OCRNL", dwRemain, &dwRemain);
	win_flagname(ONLCR, "ONLCR", dwRemain, &dwRemain);
	msvc_printf("[0x%x])\n", dwRemain);
}

/****************************************************/

int 
term_TIOCGWINSZ(WIN_VNODE *Node, WIN_WINSIZE *WinSize)
{
	int result = 0;

	if (!vfs_TIOCGWINSZ(Node, WinSize)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
term_TIOCSWINSZ(WIN_VNODE *Node, WIN_WINSIZE *WinSize)
{
	int result = 0;

	if (!vfs_TIOCSWINSZ(Node, WinSize)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
term_TIOCFLUSH(WIN_VNODE *Node)
{
	int result = 0;

	if (!vfs_TIOCFLUSH(Node)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
term_TIOCDRAIN(WIN_VNODE *Node)
{
	int result = 0;

	if (!vfs_TIOCDRAIN(Node)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
term_TIOCGETA(WIN_VNODE *Node, WIN_TERMIO *Mode)
{
	int result = 0;

	if (!vfs_TIOCGETA(Node, Mode)){
		result -= errno_posix(GetLastError());
	}else{
		*Mode = __Terminals[Node->Index].Mode;
	}
	return(result);
}
int 
term_TIOCSETA(WIN_VNODE *Node, WIN_TERMIO *Mode)
{
	int result = 0;

//termio_debug(Mode, "term_TIOCSETA");
	if (!vfs_TIOCSETA(Node, Mode, FALSE, FALSE)){
		result -= errno_posix(GetLastError());
	}else{
		__Terminals[Node->Index].Mode = *Mode;
	}
	return(result);
}
int 
term_TIOCSETAW(WIN_VNODE *Node, WIN_TERMIO *Mode)
{
	int result = 0;

//termio_debug(Mode, "term_TIOCSETAW");
	if (!vfs_TIOCSETA(Node, Mode, FALSE, TRUE)){
		result -= errno_posix(GetLastError());
	}else{
		__Terminals[Node->Index].Mode = *Mode;
	}
	return(result);
}
int 
term_TIOCSETAF(WIN_VNODE *Node, WIN_TERMIO *Mode)
{
	int result = 0;

//termio_debug(Mode, "term_TIOCSETAF");
	if (!vfs_TIOCSETA(Node, Mode, TRUE, TRUE)){
		result -= errno_posix(GetLastError());
	}else{
		__Terminals[Node->Index].Mode = *Mode;
	}
	return(result);
}
int 
term_TIOCSCTTY(WIN_VNODE *Node, WIN_TASK *Task)
{
	int result = 0;

	if (!vfs_TIOCSCTTY(Node, Task)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
term_PTMGET(WIN_VNODE *Node, WIN_TASK *Task, struct ptmget *ptm)
{
	int result = 0;
	WIN_VNODE vnMaster = {0};
	WIN_VNODE vnSlave = {0};

	if (!vfs_PTMGET(Node, &vnMaster, &vnSlave)){
		result -= errno_posix(GetLastError());
	}else{
		/* controlling terminal (master) */
		ptm->cfd = fd_posix(Task, &vnMaster, 0);
		win_strncpy(ptm->cn, DEVICE(vnMaster.DeviceId)->Name, 16);
		/* raw serial device (slave) */
		ptm->sfd = fd_posix(Task, &vnSlave, 0);
		win_strncpy(ptm->sn, DEVICE(vnSlave.DeviceId)->Name, 16);
	}
	return(result);
}
int 
term_TIOCSPGRP(WIN_VNODE *Node, UINT *GroupId)
{
	int result = 0;

	if (!vfs_TIOCSPGRP(Node, *GroupId)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
term_TIOCGPGRP(WIN_VNODE *Node, UINT *Result)
{
	int result = 0;

	if (!vfs_TIOCGPGRP(Node, Result)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
term_TIOCGETD(UINT *Result)
{
//	*Result = (__CTTY->Mode.Input & ENABLE_LINE_INPUT);
	return(0);
}
int 
term_TIOCSFLAGS(WIN_VNODE *Node, UINT *Flags)
{
	int result = 0;

	if (!vfs_TIOCSFLAGS(&__Terminals[Node->Index], *Flags)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
term_TIOCGFLAGS(WIN_VNODE *Node, UINT *Result)
{
	int result = 0;

	if (!vfs_TIOCGFLAGS(&__Terminals[Node->Index], Result)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}

/****************************************************/

int 
term_ioctl(WIN_TASK *Task, int fd, unsigned long request, va_list args)
{
	int result = 0;
	DWORD wOperation = request & 0xFF;
	WIN_VNODE *pvNode = &Task->Node[fd];

	/* tty(4)
	 */
	if (!pvNode->Access){
		result = -EBADF;
	}else switch (request){
		case TIOCFLUSH:
			result = term_TIOCFLUSH(pvNode);
			break;
		case TIOCDRAIN:
			result = term_TIOCDRAIN(pvNode);
			break;
		case PTMGET:
			result = term_PTMGET(pvNode, Task, va_arg(args, struct ptmget *));
			break;
		case TIOCSCTTY:
			result = term_TIOCSCTTY(pvNode, Task);
			break;
		case TIOCGETA:
			result = term_TIOCGETA(pvNode, va_arg(args, WIN_TERMIO *));
			break;
		case TIOCGWINSZ:
			result = term_TIOCGWINSZ(pvNode, va_arg(args, WIN_WINSIZE *));
			break;
		case TIOCSWINSZ:
			result = term_TIOCSWINSZ(pvNode, va_arg(args, WIN_WINSIZE *));
			break;
		case TIOCSETA:
			result = term_TIOCSETA(pvNode, va_arg(args, WIN_TERMIO *));
			break;
		case TIOCSETAW:
			result = term_TIOCSETAW(pvNode, va_arg(args, WIN_TERMIO *));
			break;
		case TIOCSETAF:
			result = term_TIOCSETAF(pvNode, va_arg(args, WIN_TERMIO *));
			break;
		case TIOCSPGRP:
			result = term_TIOCSPGRP(pvNode, va_arg(args, UINT *));
			break;
		case TIOCGPGRP:
			result = term_TIOCGPGRP(pvNode, va_arg(args, UINT *));
			break;
		case TIOCGETD:
			result = term_TIOCGETD(va_arg(args, UINT *));
			break;
		case TIOCSFLAGS:		/* ttyflags.exe */
			result = term_TIOCSFLAGS(pvNode, va_arg(args, UINT *));
			break;
		case TIOCGFLAGS:
			result = term_TIOCGFLAGS(pvNode, va_arg(args, UINT *));
			break;
		default:
			result = -EOPNOTSUPP;
	}
	return(result);
}
