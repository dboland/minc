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

#include <netinet/in.h>

/* https://learn.microsoft.com/en-us/windows/win32/winsock/ipproto-ip-socket-options
 */

#define WS_IP_PORTRANGE		-1	/* ftp.exe -A */

static const 
UINT IN_SOCKOPT_WIN[] = {
	0,
	WS_IP_OPTIONS,
	WS_IP_HDRINCL,
	WS_IP_TOS,
	WS_IP_TTL,
	0,
	0,
	0,
	0,
	WS_IP_MULTICAST_IF,
	WS_IP_MULTICAST_TTL,	/* 10 */
	WS_IP_MULTICAST_LOOP,
	WS_IP_ADD_MEMBERSHIP,
	WS_IP_DROP_MEMBERSHIP,
	0,
	0,
	0,
	0,
	0,
	WS_IP_PORTRANGE,
	0,			/* 20 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	WS_IP_RECVIF,		/* 30 */
	WS_IP_RECVTTL,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,			/* 40 */
	0
};
static const 
int IN_SOCKOPT_POSIX[] = {
	0,
	IP_OPTIONS,
	IP_HDRINCL,
	IP_TOS,
	IP_TTL,
	0,
	0,
	0,
	0,
	IP_MULTICAST_IF,
	IP_MULTICAST_TTL,	/* 10 */
	IP_MULTICAST_LOOP,
	IP_ADD_MEMBERSHIP,
	IP_DROP_MEMBERSHIP,
	0,
	0,
	0,
	0,
	0,
	0,
	0,			/* 20 */
	IP_RECVTTL,
	0,
	0,
	IP_RECVIF,
	0,
	0,
	0,
	0,
	0,
	0,			/* 30 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,			/* 40 */
	0
};

/****************************************************/

void *
inaddr_posix(struct sockaddr_in *addr, USHORT Family, DWORD Port, BYTE Address[4])
{
	addr->sin_len = sizeof(struct sockaddr_in);
	addr->sin_family = Family;
	addr->sin_port = Port;
	win_memcpy(&addr->sin_addr, Address, 4);
	return(addr + 1);
}
