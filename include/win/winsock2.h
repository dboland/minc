/**
 * @file winsock2.h
 * Copyright 2012, 2013 MinGW.org project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/*
 * Definitions for Windows Sockets 2
 *
 * Portions Copyright (c) 1980, 1983, 1988, 1993
 * The Regents of the University of California.  All rights reserved.
 *
 * Portions Copyright (c) 1993 by Digital Equipment Corporation.
 */

#define WINSOCK_API_LINKAGE DECLSPEC_IMPORT

#define MSG_MAXIOVLEN	16
#define MSG_PARTIAL		0x8000

#define WSABASEERR		10000

#define WSA_MAXIMUM_WAIT_EVENTS	(MAXIMUM_WAIT_OBJECTS)

#define WSADESCRIPTION_LEN      256
#define WSASYS_STATUS_LEN       128

struct ws2_sockaddr {
        u_short sa_family;
        char    sa_data[MAX_PATH];
};

typedef struct ws2_sockaddr SOCKADDR;
typedef struct ws2_sockaddr *PSOCKADDR;
typedef struct ws2_sockaddr *LPSOCKADDR;

#define WS_SOL_SOCKET              0xffff
#define WS_SO_DEBUG                0x0001
#define WS_SO_ACCEPTCONN           0x0002
#define WS_SO_REUSEADDR            0x0004
#define WS_SO_EXCLUSIVEADDRUSE     (~WS_SO_REUSEADDR)
#define WS_SO_KEEPALIVE            0x0008
#define WS_SO_DONTROUTE            0x0010
#define WS_SO_BROADCAST            0x0020
#define WS_SO_USELOOPBACK          0x0040
#define WS_SO_LINGER               0x0080
#define WS_SO_OOBINLINE            0x0100
#define WS_SO_DONTLINGER           (~WS_SO_LINGER)
#define WS_SO_SNDBUF               0x1001
#define WS_SO_RCVBUF               0x1002
#define WS_SO_SNDLOWAT             0x1003
#define WS_SO_RCVLOWAT             0x1004
#define WS_SO_SNDTIMEO             0x1005
#define WS_SO_RCVTIMEO             0x1006
#define WS_SO_ERROR                0x1007
#define WS_SO_TYPE                 0x1008
#define WS_SO_GROUP_ID             0x2001
#define WS_SO_GROUP_PRIORITY       0x2002
#define WS_SO_MAX_MSG_SIZE         0x2003
#define WS_SO_PROTOCOL_INFOA       0x2004
#define WS_SO_PROTOCOL_INFOW       0x2005
#define WS_SO_PROTOCOL_INFO        _AW(WS_SO_PROTOCOL_INFO)
#define WS_PVD_CONFIG              0x3001
#define WS_SO_CONDITIONAL_ACCEPT   0x3002

WINSOCK_API_LINKAGE INT PASCAL WSAGetLastError(VOID);

/* NT's version of iovec (sys/uio.h) */

typedef struct _WSABUF {
        unsigned long len;
        char *buf;
} WSABUF, *LPWSABUF;

/* iptypes.h */

typedef struct _SOCKET_ADDRESS {
	LPSOCKADDR lpSockaddr;
	INT iSockaddrLength;
} SOCKET_ADDRESS,*PSOCKET_ADDRESS,*LPSOCKET_ADDRESS;

/* 
 * winsock.h
 */

typedef u_int  SOCKET;

/* 
 * ws2def.h
 */

typedef struct _WSAMSG {
        LPSOCKADDR      name;
        INT             namelen;
        LPWSABUF        lpBuffers;
        DWORD           dwBufferCount;
        WSABUF          Control;
        DWORD           dwFlags;
} WSAMSG, *PWSAMSG, *LPWSAMSG;
