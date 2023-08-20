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

#include "device_namei.c"
#include "screen/screen.c"
#include "input/input.c"
#include "device_console.c"
#include "device_raw.c"
#include "device_null.c"
#include "device_pty.c"
#include "device_random.c"
#include "device_route.c"
#include "device_termio.c"
#include "driver/driver.c"
#include "device_unistd.c"
#include "device_tty.c"
#include "device_fcntl.c"
#include "device_stat.c"
#include "device_poll.c"
#include "device_statvfs.c"
#include "device_wsconsio.c"
#include "device_dkio.c"

/****************************************************/

VOID 
dev_init(WIN_DEV_CLASS Devices[])
{
	__Devices = Devices;

	config_init("mem", FS_TYPE_DEVICE, WIN_VCHR, DEV_TYPE_MEM);	/* netstat.exe */
	config_init("kmem", FS_TYPE_DEVICE, WIN_VCHR, DEV_TYPE_KMEM);
	config_init("null", FS_TYPE_DEVICE, WIN_VCHR, DEV_TYPE_NULL);
	config_init("random", FS_TYPE_DEVICE, WIN_VCHR, DEV_TYPE_RANDOM);
	config_init("urandom", FS_TYPE_DEVICE, WIN_VCHR, DEV_TYPE_URANDOM);
	config_init("route", FS_TYPE_DEVICE, WIN_VCHR, DEV_TYPE_ROUTE);
	config_init("stdin", FS_TYPE_DEVICE, WIN_VCHR, DEV_TYPE_STDIN);
	config_init("stdout", FS_TYPE_DEVICE, WIN_VCHR, DEV_TYPE_STDOUT);
	config_init("stderr", FS_TYPE_DEVICE, WIN_VCHR, DEV_TYPE_STDERR);
	config_init("swap", FS_TYPE_DEVICE, WIN_VBLK, DEV_TYPE_SWAP);

	config_init("console", FS_TYPE_CHAR, WIN_VCHR, DEV_TYPE_CONSOLE);
	config_init("tty", FS_TYPE_CHAR, WIN_VCHR, DEV_CLASS_TTY);
	config_init("ptm", FS_TYPE_CHAR, WIN_VCHR, DEV_TYPE_PTM);
	config_init("input", FS_TYPE_CHAR, WIN_VCHR, DEV_TYPE_INPUT);
	config_init("screen", FS_TYPE_CHAR, WIN_VCHR, DEV_TYPE_SCREEN);

	config_init("root", FS_TYPE_DISK, WIN_VDIR, DEV_TYPE_ROOT);

	con_init(DEVICE(DEV_TYPE_CONSOLE));
}
BOOL 
dev_found(WIN_DEVICE *Device)
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
dev_match(LPCWSTR NtName, DWORD DeviceType, WIN_CFDRIVER *Driver)
{
	BOOL bResult = FALSE;
	WIN_DEVICE *pwDevice = DEVICE(DeviceType);
	USHORT sClass = DeviceType & 0xFF00;
	USHORT sUnit = DeviceType & 0x00FF;

	while (sUnit < WIN_UNIT_MAX){
		if (!win_wcscmp(pwDevice->NtName, NtName)){
			if (!win_wcscmp(pwDevice->ClassId, Driver->ClassId)){
				bResult = TRUE;
			}else{
				pwDevice->Flags |= WIN_DVF_PORT_READY;
			}
			break;
		}else if (!pwDevice->Flags){
			pwDevice->DeviceType = DeviceType;
			pwDevice->DeviceId = sClass + sUnit;
			win_wcscpy(pwDevice->NtName, NtName);
			win_wcscpy(pwDevice->ClassId, Driver->ClassId);
			bResult = dev_found(pwDevice);
			break;
		}
		pwDevice++;
		sUnit++;
	}
	Driver->Flags = pwDevice->Flags;
	win_strcpy(Driver->Name, pwDevice->Name);
	return(bResult);
}
WIN_DEVICE *
dev_attach(DWORD DeviceType)
{
	WIN_DEVICE *pwDevice = DEVICE(DeviceType);
	USHORT sClass = DeviceType & 0xFF00;
	USHORT sUnit = DeviceType & 0x00FF;

	while (sUnit < WIN_UNIT_MAX){
		if (!pwDevice->Flags){
			pwDevice->DeviceType = DeviceType;
			pwDevice->DeviceId = sClass + sUnit;
			if (!dev_found(pwDevice)){
				msvc_printf("Warning: device 0x%x not configured\n", DeviceType);
			}
			break;
		}
		pwDevice++;
		sUnit++;
	}
	return(pwDevice);
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
