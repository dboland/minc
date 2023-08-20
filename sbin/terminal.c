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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pwd.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <unistd.h>
#include <util.h>
#include <poll.h>
#include <paths.h>

#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/param.h>	// MAXCOMLEN
#include <sys/uio.h>
#include <sys/ktrace.h>

#define TRACEPOINTS	(KTRFAC_SYSCALL | KTRFAC_SYSRET | KTRFAC_PSIG | \
				KTRFAC_STRUCT | KTRFAC_NAMEI)

int 			_verbose;
char 			_term[64] = "interix";
int			_quitting;

/* terminal control */

char	_intr = 3;	/* end of text (Ctrl-C) */
char	_eot = 4;	/* end of transmission (Ctrl-D) */

/************************************************************/

/* This terminal was written to test the pseudo terminal (pty) capabilities 
 * of libposix. In a real terminal emulation, the code below should consist
 * of initializing an actual window manager.
 */

int 
init_window(struct termios *attr, struct winsize *ws)
{
	int window;

	window = open(_PATH_CONSOLE, O_RDWR);
	if (ioctl(window, TIOCGWINSZ, ws) < 0)
		fprintf(stderr, "ioctl(TIOCGWINSZ): %s\n", window, strerror(errno));
	if (tcgetattr(window, attr) < 0)
		fprintf(stderr, "tcgetattr(%d): %s\n", window, strerror(errno));
//	attr.c_lflag &= ~ISIG;
//	attr.c_cc[VINTR] = _intr;
//	attr.c_cc[VEOF] = _eot;
//	tcsetattr(window, TCSAFLUSH, &attr);
	close(window);
}

/* In a real terminal emulation, we would not be reading from STDIN, but
 * directly from a window manager, processing window events and translating
 * them to ANSI encoded input as if coming from a terminal.
 * For details on implementation, see: ./libposix/vfs/char/input/input_unistd.c
 */

int 
write_input(int master)
{
	char buf[255];
	ssize_t count = 0;
	char *c = buf;

	if (_verbose)
		fprintf(stderr, "[in]");
	count = read(STDIN_FILENO, buf, 255);
	if (count < 0)
		return(0);
	while (count--){
		write(master, c, 1);
		if (*c == _eot){
			return(0);
		}else if (*c == _intr){
			return(0);
		}
		c++;
	}
	return(count);
}

/* In a real terminal emulation, we would not be writing to STDOUT, but
 * we would be reading the ANSI encoded output from the remote program and
 * translate them into drawing operations for the application window.
 * For details on implementation, see: ./libposix/vfs/char/screen/screen_unistd.c
 */

int 
write_output(int master)
{
	char buf[255];
	ssize_t count = 0;

	if (_verbose)
		fprintf(stderr, "[out]");
	count = read(master, buf, 255);
	if (count > 0)
		write(STDOUT_FILENO, buf, count);
	return(count);
}

/************************************************************/

void 
die(const char *msg, ...)
{
	va_list args;

	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
	exit(-1);
}
void 
sig(int signum)
{
	printf("%s: %s\n", __progname, strsignal(signum));
}
int 
shell(void)
{
	const struct passwd *pwd;
	char *args[] = {"/bin/ksh", "-l", NULL};
	uid_t uid = getuid();
	char *home = "/";
	char *sh;

	if (pwd = getpwuid(uid)){
		setenv("USER", pwd->pw_name, 1);
		setenv("HOME", pwd->pw_dir, 1);
		home = pwd->pw_dir;
		args[0] = pwd->pw_shell;
	}else{
		fprintf(stderr, "getpwuid(%d): %s\n", uid, strerror(errno));
	}

	if (sh = getenv("SHELL"))
		args[0] = sh;

	if (chdir(home) < 0)
		fprintf(stderr, "chdir(%s): %s\n", home, strerror(errno));

	execve(*args, args, environ);
	die("execve(%s): %s\n", *args, strerror(errno));
}
int 
multiplex(int master)
{
	struct pollfd pfds[2];
	int result = -1;

	pfds[0].fd = STDIN_FILENO;
	pfds[0].events = POLLIN;
	pfds[1].fd = master;
	pfds[1].events = POLLIN;
	while (result){
		if (_verbose)
			fprintf(stderr, "[poll]");
		if (poll(pfds, 2, INFTIM) < 0)
			break;
		if (pfds[0].revents)
			result = write_input(master);
		if (pfds[1].revents)
			result = write_output(master);
	}
	return(result);
}
pid_t 
getty(struct termios *attr, struct winsize *ws)
{
	int result = -1;
	int pid;
	int master, slave;
	int status;

	/* src/lib/libutil/pty.c */
	if (openpty(&master, &slave, NULL, attr, ws) < 0)
		die("openpty(): %s\n", strerror(errno));
	switch (pid = fork()){
		case -1:
			die("fork(): %s\n", strerror(errno));
		case 0:
			setsid();
			if (ioctl(slave, TIOCSCTTY, NULL) < 0)
				die("ioctl(TIOCSCTTY): %s\n", strerror(errno));
			close(master);
			dup2(slave, STDIN_FILENO);
			dup2(slave, STDOUT_FILENO);
			dup2(slave, STDERR_FILENO);
			close(slave);
			shell();
		default:
			close(slave);
			result = multiplex(master);
			waitpid(pid, &status, 0);
			close(master);
			result = 0;
	}
	return(result);
}

/************************************************************/

int 
main(int argc, char *argv[], char *envp[])
{
	int result = -1;
	char *prog = *argv++;
	char *token;
	struct termios attr;
	struct winsize ws;

	while (token = *argv++){
		if (!strcmp(token, "-v")){
			_verbose++;
		}else if (!strcmp(token, "-t")){
			ktrace("terminal.out", KTROP_SET, TRACEPOINTS, 0);
		}
	}
	signal(SIGINT, sig);
	signal(SIGQUIT, sig);
	signal(SIGCHLD, sig);
	signal(SIGWINCH, sig);
	signal(SIGTHR, sig);
	init_window(&attr, &ws);
	result = getty(&attr, &ws);
	return(result);
}
