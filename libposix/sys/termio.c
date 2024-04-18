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

#include <sys/ttydefaults.h>
#include <sys/termios.h>

/****************************************************/

void 
termio_debug(struct termios *term, const char *lable)
{
	DWORD dwRemain;

	msvc_printf("%s:\n", lable);

	dwRemain = term->c_lflag;
	msvc_printf("  local(0x%x): ", dwRemain);
	win_flagname(ECHO, "ECHO", dwRemain, &dwRemain);
	win_flagname(ECHOCTL, "ECHOCTL", dwRemain, &dwRemain);
	win_flagname(ISIG, "ISIG", dwRemain, &dwRemain);
	win_flagname(ICANON, "ICANON", dwRemain, &dwRemain);
	win_flagname(NOKERNINFO, "NOKERNINFO", dwRemain, &dwRemain);
	msvc_printf(" remain(0x%x)\n", dwRemain);

	dwRemain = term->c_iflag;
	msvc_printf("  input(0x%x): ", dwRemain);
	win_flagname(INLCR, "INLCR", dwRemain, &dwRemain);
	win_flagname(ICRNL, "ICRNL", dwRemain, &dwRemain);
	win_flagname(IXANY, "IXANY", dwRemain, &dwRemain);
	win_flagname(IXON, "IXON", dwRemain, &dwRemain);
	win_flagname(PARMRK, "PARMRK", dwRemain, &dwRemain);
	win_flagname(BRKINT, "BRKINT", dwRemain, &dwRemain);
	win_flagname(IGNPAR, "IGNPAR", dwRemain, &dwRemain);
	msvc_printf(" remain(0x%x)\n", dwRemain);

	dwRemain = term->c_oflag;
	msvc_printf("  output(0x%x): ", dwRemain);
	win_flagname(OXTABS, "OXTABS", dwRemain, &dwRemain);
	win_flagname(OCRNL, "OCRNL", dwRemain, &dwRemain);
	win_flagname(ONLCR, "ONLCR", dwRemain, &dwRemain);
	msvc_printf(" remain(0x%x)\n", dwRemain);
}
struct termios *
termio_posix(struct termios *term, WIN_TERMIO *Mode)
{
	win_bzero(term, sizeof(struct termios));
	memcpy(term->c_cc, ttydefchars, sizeof(ttydefchars));

	term->c_ispeed = TTYDEF_SPEED;
	term->c_ospeed = TTYDEF_SPEED;

	/* don't clear eighth bit (libncurses, bitchx.exe) */
	term->c_cflag = CS8;

	if (Mode->Input & ENABLE_ECHO_INPUT){
		term->c_lflag |= ECHO;
	}
	if (Mode->Input & ENABLE_LINE_INPUT){
		term->c_lflag |= ICANON;
	}
	if (Mode->Input & ENABLE_PROCESSED_INPUT){
		term->c_lflag |= ISIG;
	}
	if (Mode->Input & WIN_IXON){
		term->c_iflag |= IXON;
	}
	if (Mode->Input & WIN_IXOFF){
		term->c_iflag |= IXOFF;
	}
	if (Mode->Input & WIN_INLCR){
		term->c_iflag |= INLCR;
	}
	if (Mode->Input & WIN_ICRNL){
		term->c_iflag |= ICRNL;
	}
	if (Mode->Output & WIN_ONLCR){
		term->c_oflag |= ONLCR;
	}
	if (Mode->Output & WIN_OXTABS){
		term->c_oflag |= OXTABS;
	}
	if (Mode->Output & WIN_OCRNL){
		term->c_oflag |= OCRNL;
	}
//termio_debug(term, "termios_posix");
	return(term);
}
WIN_TERMIO *
termio_win(WIN_TERMIO *Mode, struct termios *term)
{
	Mode->Input = __CTTY->Mode.Input & ~ENABLE_ALL_INPUT;
	Mode->Output = __CTTY->Mode.Output & ~ENABLE_ALL_OUTPUT;

	if (Mode->Output & ENABLE_VIRTUAL_TERMINAL_PROCESSING){
		Mode->Output |= DISABLE_NEWLINE_AUTO_RETURN;
	}
//termio_debug(term, "termio_win");
	if (term->c_lflag & ECHO){	/* telnet.exe */
		Mode->Input |= ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT;
	}else if (term->c_lflag & ICANON){
		Mode->Input |= ENABLE_LINE_INPUT;
	}
	if (term->c_lflag & ISIG){
		Mode->Input |= ENABLE_PROCESSED_INPUT;
	}
	if (term->c_iflag & IXON){
		Mode->Input |= WIN_IXON;
	}
	if (term->c_iflag & IXOFF){
		Mode->Input |= WIN_IXOFF;
	}
	if (term->c_iflag & INLCR){
		Mode->Input |= WIN_INLCR;
	}
	if (term->c_iflag & ICRNL){
		Mode->Input |= WIN_ICRNL;
	}
	if (term->c_oflag & ONLCR){
		Mode->Output |= WIN_ONLCR;
		Mode->Output &= ~DISABLE_NEWLINE_AUTO_RETURN;
	}
	if (term->c_oflag & OXTABS){
		Mode->Output |= WIN_OXTABS;
	}
	if (term->c_oflag & OCRNL){
		Mode->Output |= WIN_OCRNL;
	}
	return(Mode);
}

/****************************************************/

int 
tty_TIOCFLUSH(WIN_VNODE *Node)
{
	int result = 0;

	if (!vfs_TIOCFLUSH(Node)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
tty_TIOCDRAIN(WIN_VNODE *Node)
{
	int result = 0;

	if (!vfs_TIOCDRAIN(Node)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
tty_TIOCGETA(WIN_VNODE *Node, struct termios *term)
{
	int result = 0;
	WIN_TERMIO ioMode = {0};

	if (!vfs_TIOCGETA(Node, &ioMode)){
		result -= errno_posix(GetLastError());
	}else{
		ioMode.Input |= (__CTTY->Mode.Input & 0xFFFF0000);
		ioMode.Output |= (__CTTY->Mode.Output & 0xFFFF0000);
		termio_posix(term, &ioMode);
	}
	return(result);
}
int 
tty_TIOCSETA(WIN_VNODE *Node, unsigned long request, struct termios *term)
{
	int result = 0;
	WIN_TERMIO ioMode = {0};
	BOOL bFlush = FALSE;
	BOOL bDrain = FALSE;

	if (request == TIOCSETAW){
		bDrain = TRUE;
	}else if (request == TIOCSETAF){
		bFlush = TRUE;
		bDrain = TRUE;
	}
	if (!vfs_TIOCSETA(Node, termio_win(&ioMode, term), bFlush, bDrain)){
		result -= errno_posix(GetLastError());
	}else{
		__CTTY->Mode = ioMode;
	}
	return(result);
}
int 
tty_TIOCGWINSZ(WIN_VNODE *Node, WIN_WINSIZE *WinSize)
{
	int result = 0;

	if (!vfs_TIOCGWINSZ(Node, WinSize)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
tty_TIOCSWINSZ(WIN_WINSIZE *WinSize)
{
	__CTTY->WinSize = *WinSize;
	return(0);
}
int 
tty_TIOCSCTTY(WIN_VNODE *Node, WIN_TASK *Task)
{
	int result = 0;

	if (Task->Flags & PS_CONTROLT){
		result = -EPERM;
	}else if (!vfs_TIOCSCTTY(Node->Device, Task)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
tty_PTMGET(WIN_VNODE *Node, WIN_TASK *Task, struct ptmget *ptm)
{
	int result = 0;
	WIN_VNODE vnResult = {0};

	if (!vfs_PTMGET(Node, &vnResult)){
		result -= errno_posix(GetLastError());
	}else{
		/* controlling terminal (master) */
		ptm->cfd = fd_posix(Task, &vnResult, 0);
		win_strncpy(ptm->cn, vnResult.Device->Name, 16);
		/* raw serial device (slave) */
		ptm->sfd = fd_posix(Task, Node, 0);
		win_strncpy(ptm->sn, Node->Device->Name, 16);
	}
	return(result);
}
int 
tty_TIOCSPGRP(WIN_VNODE *Node, UINT *GroupId)
{
	int result = 0;

	if (!vfs_TIOCSPGRP(Node, *GroupId)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
tty_TIOCGPGRP(WIN_VNODE *Node, UINT *Result)
{
	int result = 0;

	if (!vfs_TIOCGPGRP(Node, Result)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
tty_TIOCGETD(UINT *Result)
{
	*Result = (__CTTY->Mode.Input & ENABLE_LINE_INPUT);
	return(0);
}
int 
tty_TIOCSFLAGS(WIN_VNODE *Node, UINT *Flags)
{
	int result = 0;

	if (!vfs_TIOCSFLAGS(Node, *Flags)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
tty_TIOCGFLAGS(WIN_VNODE *Node, UINT *Result)
{
	int result = 0;

	if (!vfs_TIOCGFLAGS(Node, Result)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}

/****************************************************/

int 
tty_ioctl(WIN_TASK *Task, int fd, unsigned long request, va_list args)
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
			result = tty_TIOCFLUSH(pvNode);
			break;
		case TIOCDRAIN:
			result = tty_TIOCDRAIN(pvNode);
			break;
		case PTMGET:
			result = tty_PTMGET(pvNode, Task, va_arg(args, struct ptmget *));
			break;
		case TIOCSCTTY:
			result = tty_TIOCSCTTY(pvNode, Task);
			break;
		case TIOCGETA:
			result = tty_TIOCGETA(pvNode, va_arg(args, struct termios *));
			break;
		case TIOCGWINSZ:
			result = tty_TIOCGWINSZ(pvNode, va_arg(args, WIN_WINSIZE *));
			break;
		case TIOCSWINSZ:
			result = tty_TIOCSWINSZ(va_arg(args, WIN_WINSIZE *));
			break;
		case TIOCSETA:
		case TIOCSETAW:
		case TIOCSETAF:
			result = tty_TIOCSETA(pvNode, request, va_arg(args, struct termios *));
			break;
		case TIOCSPGRP:
			result = tty_TIOCSPGRP(pvNode, va_arg(args, UINT *));
			break;
		case TIOCGPGRP:
			result = tty_TIOCGPGRP(pvNode, va_arg(args, UINT *));
			break;
		case TIOCGETD:
			result = tty_TIOCGETD(va_arg(args, UINT *));
			break;
		case TIOCSFLAGS:		/* ttyflags.exe */
			result = tty_TIOCSFLAGS(pvNode, va_arg(args, UINT *));
			break;
		case TIOCGFLAGS:
			result = tty_TIOCGFLAGS(pvNode, va_arg(args, UINT *));
			break;
		default:
			result = -EOPNOTSUPP;
	}
	return(result);
}
