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

/****************************************************/

BOOL 
disk_found(WIN_DEVICE *Device)
{
	BOOL bResult = TRUE;

	switch (Device->DeviceType){
		case DEV_TYPE_WD:
			bResult = config_found("wd", FS_TYPE_DEVICE, WIN_VBLK, Device);
			break;
		case DEV_TYPE_FDC:
			bResult = config_found("fdc", FS_TYPE_DEVICE, WIN_VBLK, Device);
			break;
		case DEV_TYPE_AHCI:
			bResult = config_found("ahci", FS_TYPE_DEVICE, WIN_VBLK, Device);
			break;
		case DEV_TYPE_SD:
			bResult = config_found("sd", FS_TYPE_DEVICE, WIN_VBLK, Device);
			break;
		case DEV_TYPE_USB:
			bResult = config_found("usb", FS_TYPE_DEVICE, WIN_VBLK, Device);
			break;
		default:
			bResult = FALSE;
	}
//VfsDebugDevice(Device, "disk_found");
	return(bResult);
}
