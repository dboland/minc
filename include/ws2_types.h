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

/* iprtrmib.h */

#define MIB_IPADDR_PRIMARY		0x0001
#define MIB_IPADDR_DYNAMIC		0x0004
#define MIB_IPADDR_DISCONNECTED		0x0008
#define MIB_IPADDR_DELETED		0x0040
#define MIB_IPADDR_TRANSIENT		0x0080

/* winsock.h */

#define WIN_MSG_NOSIGNAL	0x400		/* do not send SIGPIPE */

/* winsock2.h */

#define WSAEPERM	(WSABASEERR+1)
#define WSAENOENT	(WSABASEERR+2)
#define WSAEPIPE	(WSABASEERR+32)

#define WIN_AF_LOCAL	AF_UNIX
#define WIN_AF_INET	AF_INET
#define WIN_AF_INET6	23
#define WIN_AF_ROUTE	24

#define WIN_SCM_RIGHTS		0x01
#define WIN_SCM_TIMESTAMP	0x04
#define WIN_SCM_ACCESS		0x08

/*
 * winsock_statvfs.c
 */

typedef struct _WIN_IFDATA {
	PMIB_IFTABLE Table;
	DWORD FSType;
	DWORD DeviceType;
	WCHAR NtName[MAX_NAME];
} WIN_IFDATA;

#define WS2_SOCKET_SIZE		0x2000

/*
 * winsock_sockio.c
 */

typedef enum _WS2_ADDRTYPE {
	WS2_UNICAST,
	WS2_ANYCAST,
	WS2_MULTICAST,
	WS2_DNSSERVER,
} WS2_ADDRTYPE;