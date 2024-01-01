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
	win_flagname(ISIG, "ISIG", dwRemain, &dwRemain);
	win_flagname(ICANON, "ICANON", dwRemain, &dwRemain);
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
termio_posix(struct termios *term, DWORD Mode[2])
{
	win_bzero(term, sizeof(struct termios));
	memcpy(term->c_cc, ttydefchars, sizeof(ttydefchars));

	term->c_ispeed = TTYDEF_SPEED;
	term->c_ospeed = TTYDEF_SPEED;

	/* don't clear eighth bit (libncurses, bitchx.exe) */
	term->c_cflag = CS8;

	if (Mode[0] & ENABLE_ECHO_INPUT){
		term->c_lflag |= ECHO;
	}
	if (Mode[0] & ENABLE_LINE_INPUT){
		term->c_lflag |= ICANON;
	}
	if (Mode[0] & ENABLE_PROCESSED_INPUT){
		term->c_lflag |= ISIG;
	}
	if (Mode[0] & WIN_IXON){
		term->c_iflag |= IXON;
	}
	if (Mode[0] & WIN_IXOFF){
		term->c_iflag |= IXOFF;
	}
	if (Mode[0] & WIN_INLCR){
		term->c_iflag |= INLCR;
	}
	if (Mode[0] & WIN_ICRNL){
		term->c_iflag |= ICRNL;
	}
	if (Mode[1] & WIN_ONLCR){
		term->c_oflag |= ONLCR;
	}
	if (Mode[1] & WIN_OXTABS){
		term->c_oflag |= OXTABS;
	}
	if (Mode[1] & WIN_OCRNL){
		term->c_oflag |= OCRNL;
	}
//termio_debug(term, "termios_posix");
	return(term);
}
DWORD *
termio_win(DWORD Mode[2], struct termios *term)
{
	Mode[0] = __CTTY->Mode[0] & ~ENABLE_ALL_INPUT;
	Mode[1] = __CTTY->Mode[1] & ~ENABLE_ALL_OUTPUT;

	if (Mode[1] & ENABLE_VIRTUAL_TERMINAL_PROCESSING){
		Mode[1] |= DISABLE_NEWLINE_AUTO_RETURN;
	}
//termio_debug(term, "termio_win");
	if (term->c_lflag & ECHO){
		Mode[0] |= ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT;
	}else if (term->c_lflag & ICANON){
		Mode[0] |= ENABLE_LINE_INPUT;
	}
	if (term->c_lflag & ISIG){
		Mode[0] |= ENABLE_PROCESSED_INPUT;
	}
	if (term->c_iflag & IXON){
		Mode[0] |= WIN_IXON;
	}
	if (term->c_iflag & IXOFF){
		Mode[0] |= WIN_IXOFF;
	}
	if (term->c_iflag & INLCR){
		Mode[0] |= WIN_INLCR;
	}
	if (term->c_iflag & ICRNL){
		Mode[0] |= WIN_ICRNL;
	}
	if (term->c_oflag & ONLCR){
		Mode[1] |= WIN_ONLCR;
		Mode[1] &= ~DISABLE_NEWLINE_AUTO_RETURN;
	}
	if (term->c_oflag & OXTABS){
		Mode[1] |= WIN_OXTABS;
	}
	if (term->c_oflag & OCRNL){
		Mode[1] |= WIN_OCRNL;
	}
	return(Mode);
}

/****************************************************/

int 
tty_TIOCGETA(WIN_TASK *Task, WIN_VNODE *Node, WORD Operation, struct termios *term)
{
	int result = 0;
	DWORD dwMode[2] = {0};

	if (!vfs_TIOCGETA(Node, dwMode)){
		result -= errno_posix(GetLastError());
	}else{
		dwMode[0] |= (__CTTY->Mode[0] & 0xFFFF0000);
		dwMode[1] |= (__CTTY->Mode[1] & 0xFFFF0000);
		termio_posix(term, dwMode);
	}
	return(result);
}
int 
tty_TIOCSETA(WIN_TASK *Task, WIN_VNODE *Node, WORD Operation, struct termios *term)
{
	int result = 0;
	DWORD dwMode[2] = {0};

	if (!vfs_TIOCTL(Node, Operation, termio_win(dwMode, term))){
		result -= errno_posix(GetLastError());
	}else{
		__CTTY->Mode[0] = dwMode[0];
		__CTTY->Mode[1] = dwMode[1];
	}
	return(result);
}
int 
tty_TIOCSCTTY(WIN_TASK *Task, WIN_VNODE *Node)
{
	int result = 0;

	if (!vfs_TIOCSCTTY(Node->Device, Task)){
		result -= errno_posix(GetLastError());
	}
	return(result);
}
int 
tty_PTMGET(WIN_TASK *Task, WIN_VNODE *Node, struct ptmget *ptm)
{
	int result = 0;
	WIN_PTMGET ptmGet = {0};

	if (!vfs_PTMGET(Node->Device, &ptmGet)){
		result -= errno_posix(GetLastError());
	}else{
		/* controlling terminal (master) */
		ptm->cfd = fd_posix(Task, &ptmGet.Master, 0);
		win_strncpy(ptm->cn, ptmGet.Master.Device->Name, 16);
		/* raw serial device (slave) */
		ptm->sfd = fd_posix(Task, &ptmGet.Slave, 0);
		win_strncpy(ptm->sn, ptmGet.Slave.Device->Name, 16);
	}
	return(result);
}
int 
tty_TIOCTL(WIN_TASK *Task, WIN_VNODE *Node, WORD Operation, PVOID Param)
{
	int result = 0;

	if (!vfs_TIOCTL(Node, Operation, Param)){
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

	if (!pvNode->Access){
		result = -EBADF;
	}else switch (request){
		case PTMGET:
			result = tty_PTMGET(Task, pvNode, va_arg(args, struct ptmget *));
			break;
		case TIOCSCTTY:
			result = tty_TIOCSCTTY(Task, pvNode);
			break;
		case TIOCGETA:
			result = tty_TIOCGETA(Task, pvNode, wOperation, va_arg(args, struct termios *));
			break;
		case TIOCSETA:
		case TIOCSETAW:
		case TIOCSETAF:
			result = tty_TIOCSETA(Task, pvNode, wOperation, va_arg(args, struct termios *));
			break;
		default:
			result = tty_TIOCTL(Task, pvNode, wOperation, va_arg(args, PVOID));
	}
	return(result);
}
