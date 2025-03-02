/*
 * Source: https://raw.githubusercontent.com/wine-mirror/wine/refs/heads/master/include/ws2ipdef.h
 * Copyright (C) 2009 Robert Shearman
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#define WS_IP_OPTIONS                   1
#define WS_IP_HDRINCL                   2
#define WS_IP_TOS                       3
#define WS_IP_TTL                       4
#define WS_IP_MULTICAST_IF              9
#define WS_IP_MULTICAST_TTL             10
#define WS_IP_MULTICAST_LOOP            11
#define WS_IP_ADD_MEMBERSHIP            12
#define WS_IP_DROP_MEMBERSHIP           13
#define WS_IP_DONTFRAGMENT              14
#define WS_IP_ADD_SOURCE_MEMBERSHIP     15
#define WS_IP_DROP_SOURCE_MEMBERSHIP    16
#define WS_IP_BLOCK_SOURCE              17
#define WS_IP_UNBLOCK_SOURCE            18
#define WS_IP_PKTINFO                   19
#define WS_IP_HOPLIMIT                  21
#define WS_IP_RECVTTL                   21
#define WS_IP_RECEIVE_BROADCAST         22
#define WS_IP_RECVIF                    24
#define WS_IP_RECVDSTADDR               25
#define WS_IP_IFLIST                    28
#define WS_IP_ADD_IFLIST                29
#define WS_IP_DEL_IFLIST                30
#define WS_IP_UNICAST_IF                31
#define WS_IP_RTHDR                     32
#define WS_IP_GET_IFLIST                33
#define WS_IP_RECVRTHDR                 38
#define WS_IP_TCLASS                    39
#define WS_IP_RECVTCLASS                40
#define WS_IP_RECVTOS                   40
#define WS_IP_ORIGINAL_ARRIVAL_IF       47
#define WS_IP_ECN                       50
#define WS_IP_PKTINFO_EX                51
#define WS_IP_WFP_REDIRECT_RECORDS      60
#define WS_IP_WFP_REDIRECT_CONTEXT      70
#define WS_IP_MTU_DISCOVER              71
#define WS_IP_MTU                       73
#define WS_IP_NRT_INTERFACE             74
#define WS_IP_RECVERR                   75
#define WS_IP_USER_MTU                  76

#define WS_IPV6_OPTIONS                    1
#define WS_IPV6_HOPOPTS                    1
#define WS_IPV6_HDRINCL                    2
#define WS_IPV6_UNICAST_HOPS               4
#define WS_IPV6_MULTICAST_IF               9
#define WS_IPV6_MULTICAST_HOPS             10
#define WS_IPV6_MULTICAST_LOOP             11
#define WS_IPV6_ADD_MEMBERSHIP             12
#define WS_IPV6_JOIN_GROUP                 WS_IPV6_ADD_MEMBERSHIP
#define WS_IPV6_DROP_MEMBERSHIP            13
#define WS_IPV6_LEAVE_GROUP                WS_IPV6_DROP_MEMBERSHIP
#define WS_IPV6_DONTFRAG                   14
#define WS_IPV6_PKTINFO                    19
#define WS_IPV6_HOPLIMIT                   21
#define WS_IPV6_PROTECTION_LEVEL           23
#define WS_IPV6_RECVIF                     24
#define WS_IPV6_RECVDSTADDR                25
#define WS_IPV6_CHECKSUM                   26
#define WS_IPV6_V6ONLY                     27
#define WS_IPV6_IFLIST                     28
#define WS_IPV6_ADD_IFLIST                 29
#define WS_IPV6_DEL_IFLIST                 30
#define WS_IPV6_UNICAST_IF                 31
#define WS_IPV6_RTHDR                      32
#define WS_IPV6_GET_IFLIST                 33
#define WS_IPV6_RECVRTHDR                  38
#define WS_IPV6_TCLASS                     39
#define WS_IPV6_RECVTCLASS                 40
#define WS_IPV6_ECN                        50
#define WS_IPV6_PKTINFO_EX                 51
#define WS_IPV6_WFP_REDIRECT_RECORDS       60
#define WS_IPV6_WFP_REDIRECT_CONTEXT       70
#define WS_IPV6_MTU_DISCOVER               71
#define WS_IPV6_MTU                        72
#define WS_IPV6_NRT_INTERFACE              74
#define WS_IPV6_RECVERR                    75
#define WS_IPV6_USER_MTU                   76
