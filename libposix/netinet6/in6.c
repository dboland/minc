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

/* https://learn.microsoft.com/en-us/windows/win32/winsock/ipproto-ipv6-socket-options
 * man: ip6(4)
 */

/* ping6.exe */

#define WS_IPV6_USE_MIN_MTU	-1
#define WS_IPV6_RECVHOPOPTS	-1
#define WS_IPV6_PORTRANGE	-1
#define WS_ICMP6_FILTER		-1
#define WS_IPV6_RECVDSTOPTS	-1
#define WS_IPV6_RECVHOPLIMIT	WS_IPV6_HOPLIMIT
#define WS_IPV6_RECVPKTINFO	WS_IPV6_PKTINFO

static const 
int IN6_SOCKOPT_WIN[] = {
	0,
	0,
	0,
	0,
	WS_IPV6_UNICAST_HOPS,
	0,
	0,
	0,
	0,
	WS_IPV6_MULTICAST_IF,
	WS_IPV6_MULTICAST_HOPS,		/* 10 */
	WS_IPV6_MULTICAST_LOOP,
	WS_IPV6_JOIN_GROUP,
	WS_IPV6_LEAVE_GROUP,
	WS_IPV6_PORTRANGE,
	0,
	0,
	0,
	WS_ICMP6_FILTER,
	0,
	0,			/* 20 */
	0,
	0,
	0,
	0,
	0,
	0,
	WS_IPV6_CHECKSUM,
	WS_IPV6_V6ONLY,
	0,
	0,			/* 30 */
	0,
	0,
	0,
	0,
	0,
	WS_IPV6_RECVPKTINFO,
	WS_IPV6_RECVHOPLIMIT,
	WS_IPV6_RECVRTHDR,
	WS_IPV6_RECVHOPOPTS,
	WS_IPV6_RECVDSTOPTS,	/* 40 */
	0,
	WS_IPV6_USE_MIN_MTU,
	0,
	0,
	0,
	WS_IPV6_PKTINFO,
	WS_IPV6_HOPLIMIT,
	0,
	WS_IPV6_HOPOPTS,
	0,			/* 50 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,			/* 60 */
	0
};
static const 
UINT IN6_SOCKOPT_POSIX[] = {
	0,
	0,
	0,
	0,
	IPV6_UNICAST_HOPS,
	0,
	0,
	0,
	0,
	IPV6_MULTICAST_IF,
	IPV6_MULTICAST_HOPS,	/* 10 */
	IPV6_MULTICAST_LOOP,
	IPV6_JOIN_GROUP,
	IPV6_LEAVE_GROUP,
	0,
	0,
	0,
	0,
	0,
	IPV6_PKTINFO,
	0,			/* 20 */
	IPV6_HOPLIMIT,
	0,
	0,
	0,
	0,
	IPV6_CHECKSUM,
	IPV6_V6ONLY,
	0,
	0,
	0,	/* 30 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	IPV6_RECVRTHDR,
	0,
	0,	/* 40 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,	/* 50 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,	/* 60 */
	0
};
