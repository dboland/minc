/**
 * @file iphlpapi.h
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

#include "iptypes.h"

/* RFC: https://www.ietf.org/rfc/rfc1354.txt */

/*
 * iprtrmib.h
 */

#define ANY_SIZE 1
#define MAXLEN_PHYSADDR 8
#define MAXLEN_IFDESCR 256
#define MAX_INTERFACE_NAME_LEN 256

#define MIB_IPNET_TYPE_OTHER 1
#define MIB_IPNET_TYPE_INVALID 2
#define MIB_IPNET_TYPE_DYNAMIC 3
#define MIB_IPNET_TYPE_STATIC 4

#define MIB_IP_FORWARDING		1
#define MIB_IP_NOT_FORWARDING		2
#define MIB_USE_CURRENT_FORWARDING	0xFFFF

typedef struct {
  DWORD dwAddr;
  DWORD dwIndex;
  DWORD dwMask;
  DWORD dwBCastAddr;
  DWORD dwReasmSize;
  unsigned short unused1;
  unsigned short wType;
} MIB_IPADDRROW, *PMIB_IPADDRROW;

typedef struct {
  DWORD dwNumEntries;
  MIB_IPADDRROW table[ANY_SIZE];
} MIB_IPADDRTABLE, *PMIB_IPADDRTABLE;

typedef struct {
  WCHAR wszName[MAX_INTERFACE_NAME_LEN];
  DWORD dwIndex;
  DWORD dwType;
  DWORD dwMtu;
  DWORD dwSpeed;
  DWORD dwPhysAddrLen;
  BYTE bPhysAddr[MAXLEN_PHYSADDR];
  DWORD dwAdminStatus;
  DWORD dwOperStatus;
  DWORD dwLastChange;
  DWORD dwInOctets;
  DWORD dwInUcastPkts;
  DWORD dwInNUcastPkts;
  DWORD dwInDiscards;
  DWORD dwInErrors;
  DWORD dwInUnknownProtos;
  DWORD dwOutOctets;
  DWORD dwOutUcastPkts;
  DWORD dwOutNUcastPkts;
  DWORD dwOutDiscards;
  DWORD dwOutErrors;
  DWORD dwOutQLen;
  DWORD dwDescrLen;
  BYTE  bDescr[MAXLEN_IFDESCR];
} MIB_IFROW, *PMIB_IFROW;

typedef struct {
  DWORD dwNumEntries;
  MIB_IFROW table[ANY_SIZE];
} MIB_IFTABLE, *PMIB_IFTABLE;

typedef struct {
  DWORD dwState;
  DWORD dwLocalAddr;
  DWORD dwLocalPort;
  DWORD dwRemoteAddr;
  DWORD dwRemotePort;
} MIB_TCPROW, *PMIB_TCPROW;

typedef struct {
  DWORD dwNumEntries;
  MIB_TCPROW table[ANY_SIZE];
} MIB_TCPTABLE, *PMIB_TCPTABLE;

typedef struct {
  DWORD dwForwardDest;
  DWORD dwForwardMask;
  DWORD dwForwardPolicy;
  DWORD dwForwardNextHop;
  DWORD dwForwardIfIndex;
  DWORD dwForwardType;
  DWORD dwForwardProto;
  DWORD dwForwardAge;
  DWORD dwForwardNextHopAS;
  DWORD dwForwardMetric1;
  DWORD dwForwardMetric2;
  DWORD dwForwardMetric3;
  DWORD dwForwardMetric4;
  DWORD dwForwardMetric5;
} MIB_IPFORWARDROW, *PMIB_IPFORWARDROW;

typedef struct {
  DWORD dwNumEntries;
  MIB_IPFORWARDROW table[ANY_SIZE];
} MIB_IPFORWARDTABLE, *PMIB_IPFORWARDTABLE;

typedef struct {
  DWORD dwIndex;
  DWORD dwPhysAddrLen;
  BYTE bPhysAddr[MAXLEN_PHYSADDR];
  DWORD dwAddr;
  DWORD dwType;
} MIB_IPNETROW, *PMIB_IPNETROW;

typedef struct {
  DWORD dwNumEntries;
  MIB_IPNETROW table[ANY_SIZE];
} MIB_IPNETTABLE, *PMIB_IPNETTABLE;

/*
 * ipexport.h
 */

#define MAX_ADAPTER_NAME 128

typedef struct {
  ULONG Index;
  WCHAR  Name[MAX_ADAPTER_NAME];
} IP_ADAPTER_INDEX_MAP, *PIP_ADAPTER_INDEX_MAP;

typedef struct {
  LONG NumAdapters;
  IP_ADAPTER_INDEX_MAP Adapter[ANY_SIZE];
} IP_INTERFACE_INFO, *PIP_INTERFACE_INFO;

/*
 * ipmib.h
 */

#define MIB_IPROUTE_TYPE_OTHER		1
#define MIB_IPROUTE_TYPE_INVALID	2
#define MIB_IPROUTE_TYPE_DIRECT		3
#define MIB_IPROUTE_TYPE_INDIRECT	4

/*
 * ipifcons.h
 */

#define MIN_IF_TYPE 1
#define IF_TYPE_OTHER 1
#define IF_TYPE_REGULAR_1822 2
#define IF_TYPE_HDH_1822 3
#define IF_TYPE_DDN_X25 4
#define IF_TYPE_RFC877_X25 5
#define IF_TYPE_ETHERNET_CSMACD 6
#define IF_TYPE_IS088023_CSMACD 7
#define IF_TYPE_ISO88024_TOKENBUS 8
#define IF_TYPE_ISO88025_TOKENRING 9
#define IF_TYPE_ISO88026_MAN 10
#define IF_TYPE_STARLAN 11
#define IF_TYPE_PROTEON_10MBIT 12
#define IF_TYPE_PROTEON_80MBIT 13
#define IF_TYPE_HYPERCHANNEL 14
#define IF_TYPE_FDDI 15
#define IF_TYPE_LAP_B 16
#define IF_TYPE_SDLC 17
#define IF_TYPE_DS1 18
#define IF_TYPE_E1 19
#define IF_TYPE_BASIC_ISDN 20
#define IF_TYPE_PRIMARY_ISDN 21
#define IF_TYPE_PROP_POINT2POINT_SERIAL 22
#define IF_TYPE_PPP 23
#define IF_TYPE_SOFTWARE_LOOPBACK 24
#define IF_TYPE_EON 25
#define IF_TYPE_ETHERNET_3MBIT 26
#define IF_TYPE_NSIP 27
#define IF_TYPE_SLIP 28
#define IF_TYPE_ULTRA 29
#define IF_TYPE_DS3 30
#define IF_TYPE_SIP 31
#define IF_TYPE_FRAMERELAY 32
#define IF_TYPE_RS232 33
#define IF_TYPE_PARA 34
#define IF_TYPE_ARCNET 35
#define IF_TYPE_ARCNET_PLUS 36
#define IF_TYPE_ATM 37
#define IF_TYPE_MIO_X25 38
#define IF_TYPE_SONET 39
#define IF_TYPE_X25_PLE 40
#define IF_TYPE_ISO88022_LLC 41
#define IF_TYPE_LOCALTALK 42
#define IF_TYPE_SMDS_DXI 43
#define IF_TYPE_FRAMERELAY_SERVICE 44
#define IF_TYPE_V35 45
#define IF_TYPE_HSSI 46
#define IF_TYPE_HIPPI 47
#define IF_TYPE_MODEM 48
#define IF_TYPE_AAL5 49
#define IF_TYPE_SONET_PATH 50
#define IF_TYPE_SONET_VT 51
#define IF_TYPE_SMDS_ICIP 52
#define IF_TYPE_PROP_VIRTUAL 53
#define IF_TYPE_PROP_MULTIPLEXOR 54
#define IF_TYPE_IEEE80212 55
#define IF_TYPE_FIBRECHANNEL 56
#define IF_TYPE_HIPPIINTERFACE 57
#define IF_TYPE_FRAMERELAY_INTERCONNECT 58
#define IF_TYPE_AFLANE_8023 59
#define IF_TYPE_AFLANE_8025 60
#define IF_TYPE_CCTEMUL 61
#define IF_TYPE_FASTETHER 62
#define IF_TYPE_ISDN 63
#define IF_TYPE_V11 64
#define IF_TYPE_V36 65
#define IF_TYPE_G703_64K 66
#define IF_TYPE_G703_2MB 67
#define IF_TYPE_QLLC 68
#define IF_TYPE_FASTETHER_FX 69
#define IF_TYPE_CHANNEL 70
#define IF_TYPE_IEEE80211 71
#define IF_TYPE_IBM370PARCHAN 72
#define IF_TYPE_ESCON 73
#define IF_TYPE_DLSW 74
#define IF_TYPE_ISDN_S 75
#define IF_TYPE_ISDN_U 76
#define IF_TYPE_LAP_D 77
#define IF_TYPE_IPSWITCH 78
#define IF_TYPE_RSRB 79
#define IF_TYPE_ATM_LOGICAL 80
#define IF_TYPE_DS0 81
#define IF_TYPE_DS0_BUNDLE 82
#define IF_TYPE_BSC 83
#define IF_TYPE_ASYNC 84
#define IF_TYPE_CNR 85
#define IF_TYPE_ISO88025R_DTR 86
#define IF_TYPE_EPLRS 87
#define IF_TYPE_ARAP 88
#define IF_TYPE_PROP_CNLS 89
#define IF_TYPE_HOSTPAD 90
#define IF_TYPE_TERMPAD 91
#define IF_TYPE_FRAMERELAY_MPI 92
#define IF_TYPE_X213 93
#define IF_TYPE_ADSL 94
#define IF_TYPE_RADSL 95
#define IF_TYPE_SDSL 96
#define IF_TYPE_VDSL 97
#define IF_TYPE_ISO88025_CRFPRINT 98
#define IF_TYPE_MYRINET 99
#define IF_TYPE_VOICE_EM 100
#define IF_TYPE_VOICE_FXO 101
#define IF_TYPE_VOICE_FXS 102
#define IF_TYPE_VOICE_ENCAP 103
#define IF_TYPE_VOICE_OVERIP 104
#define IF_TYPE_ATM_DXI 105
#define IF_TYPE_ATM_FUNI 106
#define IF_TYPE_ATM_IMA 107
#define IF_TYPE_PPPMULTILINKBUNDLE 108
#define IF_TYPE_IPOVER_CDLC 109
#define IF_TYPE_IPOVER_CLAW 110
#define IF_TYPE_STACKTOSTACK 111
#define IF_TYPE_VIRTUALIPADDRESS 112
#define IF_TYPE_MPC 113
#define IF_TYPE_IPOVER_ATM 114
#define IF_TYPE_ISO88025_FIBER 115
#define IF_TYPE_TDLC 116
#define IF_TYPE_GIGABITETHERNET 117
#define IF_TYPE_HDLC 118
#define IF_TYPE_LAP_F 119
#define IF_TYPE_V37 120
#define IF_TYPE_X25_MLP 121
#define IF_TYPE_X25_HUNTGROUP 122
#define IF_TYPE_TRANSPHDLC 123
#define IF_TYPE_INTERLEAVE 124
#define IF_TYPE_FAST 125
#define IF_TYPE_IP 126			//
#define IF_TYPE_DOCSCABLE_MACLAYER 127
#define IF_TYPE_DOCSCABLE_DOWNSTREAM 128
#define IF_TYPE_DOCSCABLE_UPSTREAM 129
#define IF_TYPE_A12MPPSWITCH 130
#define IF_TYPE_TUNNEL 131			//
#define IF_TYPE_COFFEE 132
#define IF_TYPE_CES 133
#define IF_TYPE_ATM_SUBINTERFACE 134
#define IF_TYPE_L2_VLAN 135		//
#define IF_TYPE_L3_IPVLAN 136		//
#define IF_TYPE_L3_IPXVLAN 137		//
#define IF_TYPE_DIGITALPOWERLINE 138
#define IF_TYPE_MEDIAMAILOVERIP 139
#define IF_TYPE_DTM 140
#define IF_TYPE_DCN 141
#define IF_TYPE_IPFORWARD 142
#define IF_TYPE_MSDSL 143
#define IF_TYPE_IEEE1394 144
#define IF_TYPE_RECEIVE_ONLY 145
#define MAX_IF_TYPE 145
#define IF_ACCESS_LOOPBACK 1
#define IF_ACCESS_BROADCAST 2
#define IF_ACCESS_POINTTOPOINT 3
#define IF_ACCESS_POINTTOMULTIPOINT 4
#define IF_CHECK_NONE 0x00
#define IF_CHECK_MCAST 0x01
#define IF_CHECK_SEND 0x02
#define IF_CONNECTION_DEDICATED 1
#define IF_CONNECTION_PASSIVE 2
#define IF_CONNECTION_DEMAND 3
#define IF_ADMIN_STATUS_UP 1
#define IF_ADMIN_STATUS_DOWN 2
#define IF_ADMIN_STATUS_TESTING 3
#define IF_OPER_STATUS_NON_OPERATIONAL 0
#define IF_OPER_STATUS_UNREACHABLE 1
#define IF_OPER_STATUS_DISCONNECTED 2
#define IF_OPER_STATUS_CONNECTING 3
#define IF_OPER_STATUS_CONNECTED 4
#define IF_OPER_STATUS_OPERATIONAL 5
#define MIB_IF_ADMIN_STATUS_UP 1
#define MIB_IF_ADMIN_STATUS_DOWN 2
#define MIB_IF_ADMIN_STATUS_TESTING 3
#define MIB_IF_OPER_STATUS_NON_OPERATIONAL 0
#define MIB_IF_OPER_STATUS_UNREACHABLE 1
#define MIB_IF_OPER_STATUS_DISCONNECTED 2
#define MIB_IF_OPER_STATUS_CONNECTING 3
#define MIB_IF_OPER_STATUS_CONNECTED 4
#define MIB_IF_OPER_STATUS_OPERATIONAL 5
#define MIB_IF_TYPE_OTHER 1
#define MIB_IF_TYPE_ETHERNET 6
#define MIB_IF_TYPE_TOKENRING 9
#define MIB_IF_TYPE_FDDI 15
#define MIB_IF_TYPE_PPP 23
#define MIB_IF_TYPE_LOOPBACK 24
#define MIB_IF_TYPE_SLIP 28

DWORD WINAPI GetNetworkParams(PFIXED_INFO,PULONG);
DWORD WINAPI GetIfEntry(PMIB_IFROW);
