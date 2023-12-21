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

#include <net/route.h>

#define IFMSGLEN	(sizeof(struct if_msghdr) + sizeof(struct sockaddr_dl) + 24)
#define IFAMSGLEN	(sizeof(struct ifa_msghdr) + (sizeof(struct sockaddr_in) * 3))
#define RTMSGLEN	(sizeof(struct rt_msghdr) + (sizeof(struct sockaddr_in) * 2))
#define INMSGLEN	(sizeof(struct ifa_msghdr) + sizeof(struct sockaddr_in) + sizeof(struct sockaddr_dl) + 24)

/****************************************************/

void *
ifmsg_posix(void *buf, MIB_IFROW *Interface)
{
	struct if_msghdr *hdr = buf;
	struct if_data *data = &hdr->ifm_data;
	struct sockaddr_dl *addr;

	hdr->ifm_msglen = IFMSGLEN;
	hdr->ifm_version = RTM_VERSION;
	hdr->ifm_type = RTM_IFINFO;
	hdr->ifm_hdrlen = sizeof(struct if_msghdr);
	hdr->ifm_index = Interface->dwIndex;
//	hdr->ifm_tableid
	/* announce type of records added below */
	hdr->ifm_addrs = RTA_IFP | RTA_IFA;
	hdr->ifm_flags = ifflags_posix(Interface, 0xFFFFFFFF);
//	hdr->ifm_xflags
	data->ifi_type = Interface->dwType;
	data->ifi_addrlen = sizeof(struct sockaddr_dl);
	data->ifi_hdrlen = sizeof(struct if_data);
//	data->ifi_link_state
	data->ifi_mtu = Interface->dwMtu;
//	data->ifi_metric
	data->ifi_baudrate = Interface->dwSpeed;
	data->ifi_ipackets = Interface->dwInUcastPkts;
	data->ifi_ierrors = Interface->dwInErrors;
	data->ifi_opackets = Interface->dwOutUcastPkts;
	data->ifi_oerrors = Interface->dwOutErrors;
//	data->ifi_collisions
	data->ifi_ibytes = Interface->dwInOctets;
	data->ifi_obytes = Interface->dwOutOctets;
//	data->ifi_imcasts
//	data->ifi_omcasts
//	data->ifi_iqdrops
//	data->ifi_noproto
//	data->ifi_capabilities
//	data->ifi_lastchange
//	data->ifi_mclpool
	addr = buf + hdr->ifm_hdrlen;
	dladdr_posix(addr++, Interface->dwIndex, Interface->dwType, Interface->bPhysAddr, Interface->dwPhysAddrLen);
	return(addr);
}
void *
ifamsg_posix(void *buf, MIB_IFROW *Interface, MIB_IPADDRROW *Address)
{
	struct ifa_msghdr *hdr = buf;
	struct sockaddr_in *addr;
	DWORD dwBroadcast = Address->dwAddr | ~Address->dwMask;

	hdr->ifam_msglen = IFAMSGLEN;
	hdr->ifam_version = RTM_VERSION;
	hdr->ifam_type = RTM_NEWADDR;
	hdr->ifam_hdrlen = sizeof(struct ifa_msghdr);
	hdr->ifam_index = Address->dwIndex;
//	hdr->ifam_tableid
	/* records added below need to be announced here */
	hdr->ifam_addrs = RTA_NETMASK | RTA_IFA | RTA_BRD;
	hdr->ifam_flags = ifflags_posix(Interface, Address->dwMask);
//	hdr->ifam_metric
	addr = buf + hdr->ifam_hdrlen;
	/* order is important */
	inaddr_posix(addr++, 0, (BYTE *)&Address->dwMask);
	inaddr_posix(addr++, 0, (BYTE *)&Address->dwAddr);
	inaddr_posix(addr++, 0, (BYTE *)&dwBroadcast);
	return(addr);
}
void *
rtmsg_posix(void *buf, MIB_IFROW *Interface, MIB_IPFORWARDROW *Address)
{
	struct rt_msghdr *hdr = buf;
	struct rt_metrics *metrics = &hdr->rtm_rmx;
	struct sockaddr_in *addr;

	hdr->rtm_msglen = RTMSGLEN;
	hdr->rtm_version = RTM_VERSION;
	hdr->rtm_type = RTM_NEWADDR;
	hdr->rtm_hdrlen = sizeof(struct rt_msghdr);
	hdr->rtm_index = Address->dwForwardIfIndex;
//	hdr->rtm_tableid
//	hdr->rtm_priority
//	hdr->rtm_mpls
	/* address types added below */
	hdr->rtm_addrs = RTA_DST | RTA_GATEWAY;
	hdr->rtm_flags = ifflags_posix(Interface, 0xFFFFFFFF);
//	hdr->rtm_fmask
//	hdr->rtm_pid
//	hdr->rtm_seq
//	hdr->rtm_errno
//	hdr->rtm_inits
//	metrics->rmx_pksent
//	metrics->rmx_expire
//	metrics->rmx_locks
	metrics->rmx_mtu = Interface->dwMtu;
//	metrics->rmx_refcnt
//	metrics->rmx_hopcount
//	metrics->rmx_recvpipe
//	metrics->rmx_sendpipe
//	metrics->rmx_ssthresh
//	metrics->rmx_rtt
//	metrics->rmx_rttvar
	addr = buf + hdr->rtm_hdrlen;
	inaddr_posix(addr++, 0, (BYTE *)&Address->dwForwardDest);
	inaddr_posix(addr++, 0, (BYTE *)&Address->dwForwardNextHop);
	return(addr);
}
void *
inmsg_posix(void *buf, MIB_IPNETROW *Address)
{
	struct ifa_msghdr *hdr = buf;

	hdr->ifam_msglen = INMSGLEN;
	hdr->ifam_version = RTM_VERSION;
	hdr->ifam_type = RTM_NEWADDR;
	hdr->ifam_hdrlen = sizeof(struct ifa_msghdr);
	hdr->ifam_index = Address->dwIndex;
//	hdr->ifam_tableid
	hdr->ifam_addrs = RTA_IFP | RTA_IFA | RTA_DST;
	if (Address->dwType == MIB_IPNET_TYPE_DYNAMIC){
		hdr->ifam_flags = IFA_ROUTE;
	}
//	hdr->ifam_metric
	buf += hdr->ifam_hdrlen;
	buf = inaddr_posix(buf, 0, (BYTE *)&Address->dwAddr);
	buf = dladdr_posix(buf, Address->dwIndex, Address->dwType, Address->bPhysAddr, Address->dwPhysAddrLen);
	return(buf);
}
/* void 
rtaddr_posix(struct sockaddr_in *addr, DWORD Port, BYTE Address[4])
{
//	if (Row->dwForwardType == MIB_IPROUTE_TYPE_INDIRECT){

	addr->sin_len = sizeof(struct sockaddr_in);
	addr->sin_family = AF_ROUTE;
	addr->sin_port = Port;
	win_memcpy(&addr->sin_addr, Address, 4);
} */

/****************************************************/

int 
route_NET_RT_IFLIST(void *buf, size_t *size)
{
	int result = 0;
	WIN_IFDATA ifData;
	PMIB_IFROW pifRow;
	PMIB_IPADDRTABLE pifaTable;
	PMIB_IPADDRROW pifaRow;
	DWORD ifCount, ifaCount;

	if (!ws2_setvfs(&ifData, FALSE, &pifRow, &ifCount)){
		result -= errno_posix(GetLastError());
	}else if (!ws2_NET_RT_IFALIST(&pifaTable, &pifaRow, &ifaCount)){
		result -= errno_posix(GetLastError());
	}else if (!buf){
		*size = IFMSGLEN * ifCount;
		*size += IFAMSGLEN * ifaCount;
	}else while (ifCount){
		buf = ifmsg_posix(buf, pifRow);
		if (ifaCount && (pifRow->dwIndex == pifaRow->dwIndex)){
			buf = ifamsg_posix(buf, pifRow, pifaRow);
			pifaRow--;
			ifaCount--;
		}
		pifRow--;
		ifCount--;
	}
	win_free(pifaTable);
	ws2_endvfs(&ifData);
	return(result);
}
int 
route_NET_RT_OACTIVE(void *buf, size_t *size)
{
	int result = 0;
	PMIB_IPNETTABLE pinTable;
	PMIB_IPNETROW pinRow;
	DWORD dwCount;

	if (!ws2_NET_RT_OACTIVE(&pinTable, &pinRow, &dwCount)){
		result -= errno_posix(GetLastError());
	}else if (!buf){
		*size = INMSGLEN * dwCount;
	}else while (dwCount--){
		buf = inmsg_posix(buf, pinRow);
		pinRow++;
	}
	win_free(pinTable);
	return(result);
}
int 
route_NET_RT_DUMP(void *buf, size_t *size)
{
	int result = 0;
	PMIB_IPFORWARDTABLE pfwTable;
	PMIB_IPFORWARDROW pfwRow;
	DWORD dwCount = 0;
	MIB_IFROW ifRow;
	DWORD dwResult;

	if (!ws2_NET_RT_DUMP(&pfwTable, &pfwRow, &dwCount)){
		result -= errno_posix(GetLastError());
	}else if (!buf){
		*size = RTMSGLEN * dwCount;
	}else while (dwCount--){
		ifRow.dwIndex = pfwRow->dwForwardIfIndex;
		dwResult = GetIfEntry(&ifRow);
		if (dwResult == ERROR_SUCCESS){
			buf = rtmsg_posix(buf, &ifRow, pfwRow);
		}else{
			result -= errno_posix(dwResult);
			break;
		}
		pfwRow++;
	}
	win_free(pfwTable);
	return(result);
}
int 
route_NET_RT_FLAGS(const int *name, void *buf, size_t *size)
{
	int result = 0;

	switch (name[5]){
		case IFF_OACTIVE:	/* arp.exe -a */
			result = route_NET_RT_OACTIVE(buf, size);
			break;
		default:
			result = -ENOENT;
	}
	return(result);
}
