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

#define WIN_SCM_RIGHTS		0x01
#define WIN_SCM_TIMESTAMP	0x04
#define WIN_SCM_ACCESS		0x08

#define WIN_IFF_UP          0x1             /* interface is up */
#define WIN_IFF_BROADCAST   0x2             /* broadcast address valid */
#define WIN_IFF_DEBUG       0x4             /* turn on debugging */
#define WIN_IFF_LOOPBACK    0x8             /* is a loopback net */
#define WIN_IFF_POINTOPOINT 0x10            /* interface is point-to-point link */
#define WIN_IFF_NOTRAILERS  0x20            /* avoid use of trailers */
#define WIN_IFF_RUNNING     0x40            /* resources allocated */
#define WIN_IFF_NOARP       0x80            /* no address resolution protocol */
#define WIN_IFF_PROMISC     0x100           /* receive all packets */
#define WIN_IFF_ALLMULTI    0x200           /* receive all multicast packets */
#define WIN_IFF_OACTIVE     0x400           /* transmission in progress */
#define WIN_IFF_SIMPLEX     0x800           /* can't hear own transmissions */
#define WIN_IFF_LINK0       0x1000          /* per link layer defined bit */
#define WIN_IFF_LINK1       0x2000          /* per link layer defined bit */
#define WIN_IFF_LINK2       0x4000          /* per link layer defined bit */
#define WIN_IFF_MULTICAST   0x8000          /* supports multicast */

/* iprtrmib.h */

#define MIB_IPADDR_PRIMARY		0x0001
#define MIB_IPADDR_DYNAMIC		0x0004
#define MIB_IPADDR_DISCONNECTED		0x0008
#define MIB_IPADDR_DELETED		0x0040
#define MIB_IPADDR_TRANSIENT		0x0080

/* winsock.h */

#define WIN_MSG_NOSIGNAL	0x400		/* do not send SIGPIPE */

/* winsock2.h */

#define WIN_AF_UNSPEC	AF_UNSPEC
#define WIN_AF_LOCAL	AF_UNIX
#define WIN_AF_INET	AF_INET
#define WIN_AF_INET6	23
#define WIN_AF_ROUTE	24

/*
 * winsock_statvfs.c
 */

typedef struct _WIN_IFDATA {
	PVOID Table;
	PIP_ADAPTER_ADDRESSES Next;
	DWORD Index;
	DWORD Type;
	DWORD FSType;
	DWORD DeviceType;
	WCHAR NtName[MAX_NAME];
} WIN_IFDATA;

#define WS2_SOCKET_SIZE		0x2000

/*
 * winsock_if.c
 */

typedef struct _WIN_IFENUM {
	PVOID Table;
	PIP_ADAPTER_ADDRESSES Next;
} WIN_IFENUM;

typedef struct _WIN_IFENT {
	DWORD IfIndex;
	DWORD IfType;
	DWORD IfFlags;
	DWORD Mtu;
	PIP_ADAPTER_UNICAST_ADDRESS Unicast;
	DWORD AddrLen;
	BYTE PhysAddr[MAXLEN_PHYSADDR];
} WIN_IFENT;
