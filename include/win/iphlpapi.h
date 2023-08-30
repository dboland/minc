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
#include "iprtrmib.h"
#include "ipifcons.h"

/* RFC: https://www.ietf.org/rfc/rfc1354.txt */

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

DWORD WINAPI GetNetworkParams(PFIXED_INFO,PULONG);
DWORD WINAPI GetIfEntry(PMIB_IFROW);
ULONG GetIpStatistics(PMIB_IPSTATS Statistics);
