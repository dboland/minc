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

#include <sys/sockio.h>

/****************************************************/

int 
sock_SIOCGIFFLAGS(WIN_TASK *Task, struct ifreq *req)
{
	int result = -1;
	MIB_IFROW ifRow = {0};
	DWORD dwResult;

	ifRow.dwIndex = ws2_nametoindex(req->ifr_name);
	dwResult = GetIfEntry(&ifRow);
	if (dwResult != ERROR_SUCCESS){
		__errno_posix(Task, dwResult);
	}else{
		req->ifr_flags = ifflags_posix(&ifRow, 0xFFFFFFFF);
		result = 0;
	}
	return(result);
}
int 
sock_SIOCGIFMTU(WIN_TASK *Task, struct ifreq *req)
{
	int result = -1;
	MIB_IFROW ifRow = {0};
	DWORD dwResult;

	ifRow.dwIndex = ws2_nametoindex(req->ifr_name);
	dwResult = GetIfEntry(&ifRow);
	if (dwResult != ERROR_SUCCESS){
		__errno_posix(Task, dwResult);
	}else{
		req->ifr_mtu = ifRow.dwMtu;
		result = 0;
	}
	return(result);
}
int 
sock_SIOCGIFGROUP(WIN_TASK *Task, struct ifgroupreq *req)
{
	char *ifname = req->ifgr_name;
	char *result = req->ifgr_group;
	char c;

	while (c = *ifname++){
		if (c < '0' || c > '9'){
			*result++ = c;
		}
	}
	*result = 0;
	return(0);
}
int 
sock_SIOCGIFDESCR(WIN_TASK *Task, struct ifreq *req)
{
	int result = -1;
	MIB_IFROW ifRow = {0};
	DWORD dwResult;

	ifRow.dwIndex = ws2_nametoindex(req->ifr_name);
	dwResult = GetIfEntry(&ifRow);
	if (dwResult != ERROR_SUCCESS){
		__errno_posix(Task, dwResult);
	}else{
		win_strncpy(req->ifr_data, ifRow.bDescr, IFDESCRSIZE);
		result = 0;
	}
	return(result);
}
int 
sock_SIOCGIFADDR(WIN_TASK *Task, struct ifreq *req)
{
	int result = -1;
	MIB_IPADDRROW ifaRow = {0};
	struct sockaddr_in *addr = (struct sockaddr_in *)&req->ifr_addr;
	DWORD dwResult;

	ifaRow.dwIndex = ws2_nametoindex(req->ifr_name);
	dwResult = GetAddrEntry(&ifaRow);
	if (dwResult != ERROR_SUCCESS){
		__errno_posix(Task, dwResult);
	}else{
		inaddr_posix(addr, 0, (BYTE *)&ifaRow.dwAddr);
		result = 0;
	}
	return(result);
}
int 
sock_SIOCGIFNETMASK(WIN_TASK *Task, struct ifreq *req)
{
	int result = -1;
	MIB_IPADDRROW ifaRow = {0};
	struct sockaddr_in *addr = (struct sockaddr_in *)&req->ifr_addr;
	DWORD dwResult;

	ifaRow.dwIndex = ws2_nametoindex(req->ifr_name);
	dwResult = GetAddrEntry(&ifaRow);
	if (dwResult != ERROR_SUCCESS){
		__errno_posix(Task, dwResult);
	}else{
		inaddr_posix(addr, 0, (BYTE *)&ifaRow.dwMask);
		result = 0;
	}
	return(result);
}
int 
sock_SIOCGIFBRDADDR(WIN_TASK *Task, struct ifreq *req)
{
	MIB_IPADDRROW ifaRow = {0};
	int result = -1;
	struct sockaddr_in *addr = (struct sockaddr_in *)&req->ifr_addr;
	DWORD dwBroadcast;
	DWORD dwResult;

	ifaRow.dwIndex = ws2_nametoindex(req->ifr_name);
	dwResult = GetAddrEntry(&ifaRow);
	if (dwResult != ERROR_SUCCESS){
		__errno_posix(Task, dwResult);
	}else{
		dwBroadcast = ifaRow.dwAddr | ~ifaRow.dwMask;
		inaddr_posix(addr, 0, (BYTE *)&dwBroadcast);
		result = 0;
	}
	return(result);
}

/****************************************************/

int 
sock_ioctl(WIN_TASK *Task, int fd, unsigned long request, va_list args)
{
	int result = -1;

	switch (request){
		case SIOCGIFMTU:
			result = sock_SIOCGIFMTU(Task, va_arg(args, struct ifreq *));
			break;
		case SIOCGIFFLAGS:
			result = sock_SIOCGIFFLAGS(Task, va_arg(args, struct ifreq *));
			break;
		case SIOCGIFGROUP:
			result = sock_SIOCGIFGROUP(Task, va_arg(args, struct ifgroupreq *));
			break;
		case SIOCGIFDESCR:
			result = sock_SIOCGIFDESCR(Task, va_arg(args, struct ifreq *));
			break;
		case SIOCGIFADDR:
			result = sock_SIOCGIFADDR(Task, va_arg(args, struct ifreq *));
			break;
		case SIOCGIFNETMASK:
			result = sock_SIOCGIFNETMASK(Task, va_arg(args, struct ifreq *));
			break;
		case SIOCGIFBRDADDR:
			result = sock_SIOCGIFBRDADDR(Task, va_arg(args, struct ifreq *));
			break;
		default:
			__errno_posix(Task, ERROR_NOT_SUPPORTED);
	}
	return(result);
}
