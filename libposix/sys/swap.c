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

#include <sys/swap.h>

/****************************************************/

int 
swapctl_STATS(struct swapent *swap, int count)
{
	MEMORYSTATUSEX msInfo = {sizeof(MEMORYSTATUSEX), 0};
	DWORD dwPageSize = __Globals[WIN_HW_PAGESIZE].LowPart;
	DWORDLONG dwlTotal, dwlAvail, dwlUsed;
	WIN_DEVICE *pwDevice = DEVICE(DEV_TYPE_SWAP);

	win_bzero(swap, sizeof(struct swapent));
	GlobalMemoryStatusEx(&msInfo);

	dwlTotal = msInfo.ullTotalPageFile;
	dwlAvail = msInfo.ullAvailPageFile;
	dwlUsed = dwlTotal - dwlAvail;

	swap->se_dev = pwDevice->DeviceId;
	swap->se_flags = SWF_ENABLE;
	swap->se_nblks = (dwlTotal + dwPageSize - 1) / dwPageSize;
	swap->se_inuse = (dwlUsed + dwPageSize - 1) / dwPageSize;
	win_strncpy(win_stpcpy(swap->se_path, "/dev/"), pwDevice->Name, MAXPATHLEN - 5);

	return(count);
}

/****************************************************/

int
sys_swapctl(call_t call, int cmd, const void *arg, int misc)
{
	int result = 0;

	switch (cmd){
		case SWAP_NSWAP:
			result = 1;
			break;
		case SWAP_STATS:
			result = swapctl_STATS((struct swapent *)arg, misc);
			break;
		default:
			result = -EOPNOTSUPP;
	}
	return(result);
}
