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

/****************************************************/

void *
inaddr_posix(struct sockaddr_in *addr, DWORD Port, BYTE Address[4])
{
	addr->sin_len = sizeof(struct sockaddr_in);
	addr->sin_family = AF_INET;
	addr->sin_port = Port;
	win_memcpy(&addr->sin_addr, Address, 4);
	return(addr + 1);
}

/****************************************************/

int 
in_NET_INET6_IPV6_DAD_PENDING(const int *name, void *buf, size_t *size)
{
	int result = 0;
	DWORD dwResult;

	if (!ws2_NET_INET6_IPV6_DAD_PENDING(&dwResult)){
		result -= errno_posix(GetLastError());
	}else{
		*(int *)buf = dwResult;
	}
	return(result);
}
int 
in_NET_INET6_IPV6(const int *name, void *buf, size_t *size)
{
	int result = 0;

	switch (name[3]){
		case IPV6CTL_FORWARDING:	/* 1 */
			result = ip_NET_INET_IP_FORWARDING(buf, size);
			break;
		case IPV6CTL_DAD_PENDING:	/* 49 */
			result = in_NET_INET6_IPV6_DAD_PENDING(name, buf, size);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}
