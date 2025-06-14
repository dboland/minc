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

#include "../config.h"

LPSTR 
VfsNetEvents(LPSTR Buffer, LONG NetworkEvents)
{
	LPSTR psz = Buffer;
	DWORD dwRemain = NetworkEvents;

	psz += sprintf(psz, "([0x%x]", NetworkEvents);
	psz = WinFlagName(psz, FD_READ, "READ", dwRemain, &dwRemain);
	psz = WinFlagName(psz, FD_WRITE, "WRITE", dwRemain, &dwRemain);
	psz = WinFlagName(psz, FD_OOB, "OOB", dwRemain, &dwRemain);
	psz = WinFlagName(psz, FD_ACCEPT, "ACCEPT", dwRemain, &dwRemain);
	psz = WinFlagName(psz, FD_CONNECT, "CONNECT", dwRemain, &dwRemain);
	psz = WinFlagName(psz, FD_CLOSE, "CLOSE", dwRemain, &dwRemain);
	psz = WinFlagName(psz, FD_QOS_BIT, "QOS_BIT", dwRemain, &dwRemain);
	psz = WinFlagName(psz, FD_GROUP_QOS, "GROUP_QOS", dwRemain, &dwRemain);
	psz = WinFlagName(psz, FD_ROUTING_INTERFACE_CHANGE, "ROUTING_INTERFACE_CHANGE", dwRemain, &dwRemain);
	psz = WinFlagName(psz, FD_ADDRESS_LIST_CHANGE, "ADDRESS_LIST_CHANGE", dwRemain, &dwRemain);
	psz += sprintf(psz, "[0x%x])\n", dwRemain);
	return(psz);
}
