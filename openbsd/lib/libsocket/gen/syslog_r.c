/*	$OpenBSD: src/lib/libc/gen/syslog_r.c,v 1.4 2013/04/29 00:28:23 okan Exp $ */
/*
 * Copyright (c) 1983, 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/syslog.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <netdb.h>

#include <errno.h>
#include <fcntl.h>
#include <paths.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>

//extern char	*__progname;		/* Program name, from crt0. */

static void	disconnectlog_r(struct syslog_data *);	/* disconnect from syslogd */
static void	connectlog_r(struct syslog_data *);	/* (re)connect to syslogd */

void	__vsyslog_r(int pri, struct syslog_data *, size_t (*)(char *, size_t),
    const char *, va_list);

/* Reentrant version of syslog, i.e. syslog_r() */

/* PRINTFLIKE3 */
void
syslog_r(int pri, struct syslog_data *data, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vsyslog_r(pri, data, fmt, ap);
	va_end(ap);
}

void
vsyslog_r(int pri, struct syslog_data *data, const char *fmt, va_list ap)
{
	const char *ident;

	__vsyslog_r(pri, data, NULL, fmt, ap);

	/* close the socket without losing log_tag */
	ident = data->log_tag;
	closelog_r(data);
	data->log_tag = ident;
}

/*
 * This is used by both syslog_r and syslog.  The latter supplies
 * a non-NULL gettime callback for filling in the date, but we also
 * use the presence of that callback to decide whether it's safe
 * to call strerror and what the name of the caller is
 */
void
__vsyslog_r(int pri, struct syslog_data *data,
    size_t (*gettime)(char *, size_t), const char *fmt, va_list ap)
{
	int cnt;
	char ch, *p, *t;
	int fd, saved_errno, error;
#define	TBUF_LEN	2048
#define	FMT_LEN		1024
	char *stdp, tbuf[TBUF_LEN], fmt_cpy[FMT_LEN];
	int tbuf_left, fmt_left, prlen;

#define	INTERNALLOG	LOG_ERR|LOG_CONS|LOG_PERROR|LOG_PID
	/* Check for invalid bits. */
	if (pri & ~(LOG_PRIMASK|LOG_FACMASK)) {
		syslog_r(INTERNALLOG, data,
		    "syslog%s: unknown facility/priority: %x",
		    gettime != NULL ? "" : "_r", pri);
		pri &= LOG_PRIMASK|LOG_FACMASK;
	}

	/* Check priority against setlogmask values. */
	if (!(LOG_MASK(LOG_PRI(pri)) & data->log_mask))
		return;

	saved_errno = errno;

	/* Set default facility if none specified. */
	if ((pri & LOG_FACMASK) == 0)
		pri |= data->log_fac;

	p = tbuf;
	tbuf_left = TBUF_LEN;

#define	DEC()	\
	do {					\
		if (prlen < 0)			\
			prlen = 0;		\
		if (prlen >= tbuf_left)		\
			prlen = tbuf_left - 1;	\
		p += prlen;			\
		tbuf_left -= prlen;		\
	} while (0)

	prlen = snprintf(p, tbuf_left, "<%d>", pri);
	DEC();

	/* 
	 * syslogd will expand time automagically for reentrant case, and
	 * for normal case, invoke the callback to do it just do like before
	 */
	if (gettime != NULL) {
		prlen = gettime(p, tbuf_left);
		DEC();
	}

	if (data->log_stat & LOG_PERROR)
		stdp = p;
	if (data->log_tag == NULL)
		data->log_tag = __progname;
	if (data->log_tag != NULL) {
		prlen = snprintf(p, tbuf_left, "%s", data->log_tag);
		DEC();
	}
	if (data->log_stat & LOG_PID) {
		prlen = snprintf(p, tbuf_left, "[%ld]", (long)getpid());
		DEC();
	}
	if (data->log_tag != NULL) {
		if (tbuf_left > 1) {
			*p++ = ':';
			tbuf_left--;
		}
		if (tbuf_left > 1) {
			*p++ = ' ';
			tbuf_left--;
		}
	}

	/* strerror() is not reentrant */

	for (t = fmt_cpy, fmt_left = FMT_LEN; (ch = *fmt); ++fmt) {
		if (ch == '%' && fmt[1] == 'm') {
			++fmt;
			if (gettime != NULL) {
				prlen = snprintf(t, fmt_left, "%s",
				    strerror(saved_errno)); 
			} else {
				prlen = snprintf(t, fmt_left, "Error %d",
				    saved_errno); 
			}
			if (prlen < 0)
				prlen = 0;
			if (prlen >= fmt_left)
				prlen = fmt_left - 1;
			t += prlen;
			fmt_left -= prlen;
		} else if (ch == '%' && fmt[1] == '%' && fmt_left > 2) {
			*t++ = '%';
			*t++ = '%';
			fmt++;
			fmt_left -= 2;
		} else {
			if (fmt_left > 1) {
				*t++ = ch;
				fmt_left--;
			}
		}
	}
	*t = '\0';

	prlen = vsnprintf(p, tbuf_left, fmt_cpy, ap);
	DEC();
	cnt = p - tbuf;

	/* Output to stderr if requested. */
	if (data->log_stat & LOG_PERROR) {
		struct iovec iov[2];

		iov[0].iov_base = stdp;
		iov[0].iov_len = cnt - (stdp - tbuf);
		iov[1].iov_base = "\n";
		iov[1].iov_len = 1;
		(void)writev(STDERR_FILENO, iov, 2);
	}

	/* Get connected, output the message to the local logger. */
	if (!data->opened)
		openlog_r(data->log_tag, data->log_stat, 0, data);
	connectlog_r(data);

	/*
	 * If the send() failed, there are two likely scenarios:
	 *  1) syslogd was restarted
	 *  2) /dev/log is out of socket buffer space
	 * We attempt to reconnect to /dev/log to take care of
	 * case #1 and keep send()ing data to cover case #2
	 * to give syslogd a chance to empty its socket buffer.
	 */
	if ((error = send(data->log_file, tbuf, cnt, 0)) < 0) {
		if (errno != ENOBUFS) {
			disconnectlog_r(data);
			connectlog_r(data);
		}
		do {
			struct timespec rqt = { 0, 1000 };

			nanosleep(&rqt, NULL);
			if ((error = send(data->log_file, tbuf, cnt, 0)) >= 0)
				break;
		} while (errno == ENOBUFS);
	}

	/*
	 * Output the message to the console; try not to block
	 * as a blocking console should not stop other processes.
	 * Make sure the error reported is the one from the syslogd failure.
	 */
	if (error == -1 && (data->log_stat & LOG_CONS) &&
	    (fd = open(_PATH_CONSOLE, O_WRONLY|O_NONBLOCK, 0)) >= 0) {
		struct iovec iov[2];
		
		p = strchr(tbuf, '>') + 1;
		iov[0].iov_base = p;
		iov[0].iov_len = cnt - (p - tbuf);
		iov[1].iov_base = "\r\n";
		iov[1].iov_len = 2;
		(void)writev(fd, iov, 2);
		(void)close(fd);
	}
}

static void
disconnectlog_r(struct syslog_data *data)
{
	/*
	 * If the user closed the FD and opened another in the same slot,
	 * that's their problem.  They should close it before calling on
	 * system services.
	 */
	if (data->log_file != -1) {
		close(data->log_file);
		data->log_file = -1;
	}
	data->connected = 0;		/* retry connect */
}

static void
connectlog_r(struct syslog_data *data)
{
	struct sockaddr_un SyslogAddr;	/* AF_UNIX address of local logger */

	if (data->log_file == -1) {
		if ((data->log_file = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
			return;
		(void)fcntl(data->log_file, F_SETFD, FD_CLOEXEC);
	}
	if (data->log_file != -1 && !data->connected) {
		memset(&SyslogAddr, '\0', sizeof(SyslogAddr));
		SyslogAddr.sun_len = sizeof(SyslogAddr);
		SyslogAddr.sun_family = AF_UNIX;
		strlcpy(SyslogAddr.sun_path, _PATH_LOG,
		    sizeof(SyslogAddr.sun_path));
		if (connect(data->log_file, (struct sockaddr *)&SyslogAddr,
		    sizeof(SyslogAddr)) == -1) {
			(void)close(data->log_file);
			data->log_file = -1;
		} else
			data->connected = 1;
	}
}

void
openlog_r(const char *ident, int logstat, int logfac, struct syslog_data *data)
{
	if (ident != NULL)
		data->log_tag = ident;
	data->log_stat = logstat;
	if (logfac != 0 && (logfac &~ LOG_FACMASK) == 0)
		data->log_fac = logfac;

	if (data->log_stat & LOG_NDELAY)	/* open immediately */
		connectlog_r(data);

	data->opened = 1;	/* ident and facility has been set */
}

void
closelog_r(struct syslog_data *data)
{
	(void)close(data->log_file);
	data->log_file = -1;
	data->connected = 0;
	data->log_tag = NULL;
}

