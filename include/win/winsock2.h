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
 * mswsock.h
 */

typedef struct _WSAMSG {
        LPSOCKADDR      name;
        INT             namelen;
        LPWSABUF        lpBuffers;
        DWORD           dwBufferCount;
        WSABUF          Control;
        DWORD           dwFlags;
} WSAMSG, *PWSAMSG, *LPWSAMSG;

