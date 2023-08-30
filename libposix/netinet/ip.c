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

#include <netinet/in_systm.h>
#include <netinet/ip.h>

/****************************************************/

int 
ip_NET_INET_IP_FORWARDING(WIN_TASK *Task, void *buf, size_t *size)
{
	int result = 0;
	ULONG ulStatus;
	MIB_IPSTATS ipStats;

	ulStatus = GetIpStatistics(&ipStats);
	if (ulStatus != ERROR_SUCCESS){
		__errno_posix(Task, ulStatus);
		result = -1;
	}else if (ipStats.dwForwarding == MIB_IP_FORWARDING){
		*(int *)buf = 1;
	}else{
		*(int *)buf = 0;
	}
	return(result);
}
int 
ip_NET_INET_IP_DEFTTL(WIN_TASK *Task, void *buf, size_t *size)
{
	int result = -1;
	ULONG ulStatus;
	MIB_IPSTATS ipStats;

	ulStatus = GetIpStatistics(&ipStats);
	if (ulStatus != ERROR_SUCCESS){
		__errno_posix(Task, ulStatus);
	}else{
		*(int *)buf = ipStats.dwDefaultTTL;
		result = 0;
	}
	return(result);
}
int 
ip_NET_INET_IP(WIN_TASK *Task, const int *name, void *buf, size_t *size)
{
	int result = 0;

	switch (name[3]){
		case IPCTL_FORWARDING:		/* 1 */
			result = ip_NET_INET_IP_FORWARDING(Task, buf, size);
			break;
		case IPCTL_DEFTTL:		/* traceroute.exe */
			result = ip_NET_INET_IP_DEFTTL(Task, buf, size);
			break;
		default:
			__errno_posix(Task, ERROR_NOT_SUPPORTED);
			result = -1;
	}
	return(result);
}
