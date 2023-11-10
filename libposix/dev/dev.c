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

#include <stdlib.h>
#include <stdarg.h>

#include <sys/errno.h>

#include "win/windows.h"
#include "win_posix.h"
#include "msvc_posix.h"
#include "vfs_posix.h"
#include "arch_posix.h"
#include "dev_types.h"

extern WIN_DEV_CLASS	*__Devices;

#include "autoconf.c"
#include "wscons/wsconsio.c"
#include "dev_system.c"
#include "dev_cpu.c"
#include "dev_disk.c"
#include "dev_ifnet.c"
#include "dev_media.c"
#include "dev_serial.c"
#include "dev_printer.c"
#include "dev_display.c"
#include "dev_keyboard.c"
#include "dev_mouse.c"
#include "dev_storage.c"
#include "dev_usb.c"

/****************************************************/

BOOL 
dev_attach(WIN_DEVICE *Device)
{
	BOOL bResult = TRUE;
	USHORT sClass = Device->DeviceType & 0xFF00;

	switch (sClass){
		case DEV_CLASS_SYSTEM:
			bResult = system_found(Device);
			break;
		case DEV_CLASS_CPU:
			bResult = cpu_found(Device);
			break;
		case DEV_CLASS_DISK:
			bResult = disk_found(Device);
			break;
		case DEV_CLASS_IFNET:
			bResult = ifnet_found(Device);
			break;
		case DEV_CLASS_MEDIA:
			bResult = media_found(Device);
			break;
		case DEV_CLASS_SERIAL:
			bResult = serial_found(Device);
			break;
		case DEV_CLASS_PRINTER:
			bResult = printer_found(Device);
			break;
		case DEV_CLASS_DISPLAY:
			bResult = display_found(Device);
			break;
		case DEV_CLASS_KEYBOARD:
			bResult = keyboard_found(Device);
			break;
		case DEV_CLASS_MOUSE:
			bResult = mouse_found(Device);
			break;
		case DEV_CLASS_STORAGE:
			bResult = storage_found(Device);
			break;
		case DEV_CLASS_USB:
			bResult = usb_found(Device);
			break;
		default:
			bResult = FALSE;
	}
	return(bResult);
}
BOOL 
dev_activate(WIN_DEVICE *Device, WIN_VNODE *Result)
{
	if (!Device->Handle){
		Device->Handle = win_F_DUPFD(Result->Handle, TRUE);
		Device->Flags |= WIN_DVF_ACTIVE;
	}
//VfsDebugDevice(Device, "dev_activate");
	return(TRUE);
}
