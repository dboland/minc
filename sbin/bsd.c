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
#include <paths.h>
#include <fstab.h>
#include <regex.h>
#include <locale.h>
#include <dirent.h>

#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/sysctl.h>
#include <sys/uio.h>
#include <sys/ktrace.h>
#include <sys/reboot.h>
#include <sys/systm.h>
#include <sys/socket.h>
#include <sys/tty.h>

#define _KERNEL

#include <net/if.h>

#define KTR_DEFAULT (KTRFAC_SYSCALL | KTRFAC_SYSRET | KTRFAC_NAMEI | \
	KTRFAC_PSIG | KTRFAC_EMUL | KTRFAC_STRUCT | KTRFAC_INHERIT)

unsigned char		_verbose;
unsigned char		_boot;

int runcmd(char *argv[]);

/* src/sys/netinet/ip_input.c */
void tty_init(void);

/* sys/arch/i386/stand/boot/crt0.c */

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
fs_mount(void)
{
	int result = 0;
	struct fstab *tab;
	char fspec[PATH_MAX];
	struct ntfs_args args = {fspec, {0}, getuid(), getgid(), 00755, 0};
	regex_t rebuf;

	/* Yes, those are word boundaries */
	regcomp(&rebuf, "[[:<:]]noauto[[:>:]]", REG_EXTENDED);
	while (tab = getfsent()){
		if (regexec(&rebuf, tab->fs_mntops, 0, NULL, REG_EXTENDED)){
			strcpy(fspec, tab->fs_spec);
			mount(tab->fs_vfstype, tab->fs_file, 0, &args);
		}
	}
	endfsent();
	regfree(&rebuf);
	return(result);
}
int 
fs_unmount(void)
{
	int result = 0;
	struct fstab *tab;

	while (tab = getfsent()){
		unmount(tab->fs_file, MNT_FORCE);
	}
	endfsent();
	return(result);
}
int
getty(const char *path)
{
	int result = 0;
	int fd;

	/* ./lib/libutil/login_tty.c
	 */
	fd = open(path, O_RDWR);
	if (fd < 0){
		fprintf(stderr, "open(%s): %s\n", path, strerror(errno));
	}else if (ioctl(fd, TIOCSCTTY, NULL) < 0){
		fprintf(stderr, "ioctl(TIOCSCTTY): %s\n", strerror(errno));
	}else{
		dup2(fd, STDIN_FILENO);
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);
		if (fd > STDERR_FILENO)
			close(fd);
		result = 1;
	}
	return(result);
}
int 
shell(char *args[])
{
	const struct passwd *pwd = NULL;
	char *login = getlogin();
	char *home = "/";

	if (pwd = getpwnam(login)){
		setenv("USER", pwd->pw_name, 1);
		setenv("HOME", pwd->pw_dir, 1);
		home = pwd->pw_dir;
	}else{
		fprintf(stderr, "getpwnam(%s): %s\n", login, strerror(errno));
	}
	if (chdir(home) < 0){
		fprintf(stderr, "chdir(%s): %s\n", home, strerror(errno));
	}
	execve(*args, args, environ);
	fprintf(stderr, "execve(%s): %s\n", *args, strerror(errno));
	return(0);
}
int 
trpoints(const char *opts)
{
	int result = KTR_DEFAULT;

	if (!opts)
		return(result);
	if (strchr(opts, 'u'))
		result |= KTRFAC_USER;
	if (strchr(opts, 'i'))
		result |= KTRFAC_GENIO;
	return(result);
}

/************************************************************/

void 
boot(void)
{
	char *args[] = {"/sbin/init", NULL};

	tty_init();
	ifinit();
	cpu_configure();
	fs_unmount();		// fsck.exe operation?
	execve(*args, args, environ);
	fprintf(stderr, "execve(%s): %s\n", *args, strerror(errno));
}
void 
notty(void)
{
	char *root = getenv("MINCROOT");
	char *msg = "MinC is installed and working but could not configure itself.\n"
		"Please exclude %s from your anti-virus software\n"
		"and run /sbin/setup.sh as administrator.\n\n"
		"Press any key to continue . . .";
	char buf;

	fprintf(stderr, msg, root);
	read(0, &buf, 1);
}
void 
single(void)
{
	int mib[2] = {CTL_KERN, KERN_SECURELVL};
	int level = 1;
	char *args[] = {"/bin/ksh", "-l", NULL};

	tty_init();
	ifinit();
	cpu_configure();
	fs_unmount();
	fs_mount();
	sysctl(mib, 2, NULL, NULL, &level, sizeof(int));
//	close(0);
//	close(1);
//	close(2);
	(void) revoke(_PATH_CONSOLE);
	setsid();
	if (!getty(_PATH_CONSOLE)){
		notty();
		args[0] = "/bin/sh";
		args[1] = NULL;
	}
	shell(args);
}
void 
multi(void)
{
	char *args[] = {"/bin/ksh", "-l", NULL};

	setsid();
	getty(PATH_PTMDEV);
	shell(args);
}

/************************************************************/

int 
main(int argc, char *argv[], char *envp[])
{
	int mib[2] = {CTL_KERN, KERN_SECURELVL};
	size_t size = sizeof(int);
	int level = 0;
	char *progname = *argv++;
	char *token;
	pid_t pid;
	int status;

	while (token = *argv++){
		if (!strcmp(token, "-v")){
			_verbose++;
		}else if (!strcmp(token, "-b")){
			_boot++;
		}else if (!strcmp(token, "-t")){
			ktrace("boot.out", KTROP_SET, trpoints(*argv), 0);
		}
	}
//	signal(SIGINT, SIG_IGN);
//	signal(SIGCHLD, SIG_IGN);
	signal(SIGQUIT, sig);
	signal(SIGWINCH, sig);
//	signal(SIGINFO, SIG_IGN);
	sysctl(mib, 2, &level, &size, NULL, 0);
	setenv("PATH", _PATH_DEFPATH, 1);
	switch (pid = fork()){
		case -1:
			die("fork(): %s\n", strerror(errno));
		case 0:
			if (_boot)
				boot();
			else if (!level)
				single();
			else
				multi();
			break;
		default:
			waitpid(pid, &status, 0);
	}
}
