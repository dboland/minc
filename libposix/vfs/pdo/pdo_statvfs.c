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

#include <ddk/ntifs.h>		/* Installable File System */

/************************************************************/

DWORD 
DevLookupBus(LPCWSTR BusName, DWORD Class)
{
	DWORD dwResult = Class;

	if (!win_wcsncmp(BusName, L"PCI", 3)){
		dwResult |= DEV_BUS_PCI;

	}else if (!win_wcsncmp(BusName, L"USB", 3)){
		dwResult |= DEV_BUS_USB;

	}else if (!win_wcscmp(BusName, L"ACPI")){
		dwResult |= DEV_BUS_ACPI;

	}else if (!win_wcscmp(BusName, L"IDE")){
		dwResult |= DEV_BUS_IDE;

	}else if (!win_wcscmp(BusName, L"SCSI")){
		dwResult |= DEV_BUS_SCSI;

	}else if (!win_wcscmp(BusName, L"FDC")){
		dwResult |= DEV_BUS_FDC;

	}
	return(dwResult);
}
DWORD 
DevLookupClass(LPCWSTR Service, DWORD Bus)
{
	DWORD dwResult = Bus;

	if (!win_wcscmp(Service, L"Serial")){
		dwResult |= DEV_CLASS_SERIAL;

	}else if (!win_wcscmp(Service, L"Parport")){
		dwResult |= DEV_CLASS_PRINTER;

	}
	return(dwResult);
}
DWORD 
DevLookup(LPCWSTR Bus, LPCWSTR Class, LPCWSTR Service)
{
	DWORD dwResult = DEV_CLASS_DULL;

	if (!win_wcscmp(Class, L"diskdrive")){
		dwResult = DevLookupBus(Bus, DEV_CLASS_DISK);

	}else if (!win_wcscmp(Class, L"cdrom")){
		dwResult = DevLookupBus(Bus, DEV_CLASS_DISK);

	}else if (!win_wcscmp(Class, L"floppydisk")){
		dwResult = DevLookupBus(Bus, DEV_CLASS_DISK);

	}else if (!win_wcscmp(Class, L"net")){
		dwResult = DevLookupBus(Bus, DEV_CLASS_IFNET);

	}else if (!win_wcscmp(Class, L"display")){
		dwResult = DevLookupBus(Bus, DEV_CLASS_DISPLAY);

	}else if (!win_wcscmp(Class, L"keyboard")){
		dwResult = DevLookupBus(Bus, DEV_CLASS_KEYBOARD);

	}else if (!win_wcscmp(Class, L"mouse")){
		dwResult = DevLookupBus(Bus, DEV_CLASS_MOUSE);

	}else if (!win_wcscmp(Class, L"media")){
		dwResult = DevLookupBus(Bus, DEV_CLASS_MEDIA);

	}else if (!win_wcscmp(Class, L"image")){
		dwResult = DevLookupBus(Bus, DEV_CLASS_MEDIA);

	}else if (!win_wcscmp(Class, L"volume")){
		dwResult = DevLookupBus(Bus, DEV_CLASS_STORAGE);

	}else if (!win_wcscmp(Class, L"hidclass")){
		dwResult = DevLookupBus(Bus, DEV_CLASS_USB);

	}else if (!win_wcscmp(Class, L"ports")){
		dwResult = DevLookupClass(Service, DEV_BUS_ISA);

	}else if (!win_wcscmp(Service, L"usbprint")){	/* no "Printer" class for USB */
		dwResult = DEV_TYPE_USBPRINT;

	}else if (!win_wcscmp(Class, L"usb")){
		dwResult = DevLookupBus(Bus, DEV_CLASS_USB);

		/* Vista */

	}else if (!win_wcscmp(Class, L"hdc")){
		dwResult = DevLookupBus(Bus, DEV_CLASS_DISK);

	}else if (!win_wcscmp(Class, L"printqueue")){
		dwResult = DevLookupBus(Bus, DEV_CLASS_PRINTER);

	}else if (!win_wcscmp(Class, L"monitor")){
		dwResult = DevLookupBus(Bus, DEV_CLASS_DISPLAY);

	}else if (!win_wcscmp(Class, L"bluetooth")){
		dwResult = DevLookupBus(Bus, DEV_CLASS_USB);

	}else if (!win_wcscmp(Class, L"audioendpoint")){
		dwResult = DEV_TYPE_AUDIO;

	}else if (!win_wcscmp(Class, L"processor")){
		dwResult = DEV_TYPE_PROCESSOR;

	}else{
		dwResult = DevLookupBus(Bus, DEV_CLASS_SYSTEM);

	}
	return(dwResult);
}
BOOL 
DevClass(LPCWSTR ClassID, LPWSTR Result)
{
	WIN_VNODE vNode;
	DWORD dwResult;
	WIN_FLAGS wFlags = {GENERIC_READ, 0, 0, REG_SZ, FALSE};
	WIN_NAMEIDATA iNode;

	if (reg_open(reg_lookup(&iNode, REG_CLASS, ClassID), &wFlags, &vNode)){
		reg_read(&vNode, L"Class", Result, MAX_NAME, &dwResult);
		reg_close(&vNode);
		win_wcslcase(Result);
	}
}

/************************************************************/

BOOL 
pdo_statvfs(WIN_CFDATA *Config, DWORD Flags, WIN_CFDRIVER *Driver)
{
	BOOL bResult = FALSE;
	WIN_VNODE vNode = {0};
	DWORD dwSize;
	WIN_FLAGS wFlags = {GENERIC_READ, 0, 0, REG_SZ, FALSE};
	WIN_NAMEIDATA iNode;
	WCHAR szClass[MAX_GUID];
	WCHAR szText[MAX_TEXT];

	ZeroMemory(Driver, sizeof(WIN_CFDRIVER));
	if (reg_open(reg_lookup(&iNode, REG_DRIVER, Config->DosPath), &wFlags, &vNode)){
		win_wcscpy(Driver->ClassId, iNode.R);
		/* Device Class ("Components" in msinfo32.exe) */
		if (reg_read(&vNode, L"ClassGUID", szClass, MAX_GUID, &dwSize)){
			DevClass(szClass, Driver->NtClass);
		}
		/* Device Driver (*.sys) */
		if (!reg_read(&vNode, L"Service", Driver->Service, MAX_NAME, &dwSize)){
			win_wcscpy(Driver->Service, Config->BusName);
		}
		if (!reg_read(&vNode, L"LocationInformation", Driver->Location, MAX_COMMENT, &dwSize)){
			win_wcscpy(Driver->Location, Config->BusName);
		}
		if (reg_read(&vNode, L"FriendlyName", szText, MAX_TEXT, &dwSize)){
			win_wcscpy(Driver->Comment, szText);
		}else if (reg_read(&vNode, L"DeviceDesc", szText, MAX_TEXT, &dwSize)){
			win_wcscpy(Driver->Comment, szText);
		}
		bResult = reg_close(&vNode);
	}else{
		WIN_ERR("reg_open(%ls): %s\n", iNode.Resolved, win_strerror(GetLastError()));
	}
	Config->DeviceType = DevLookup(Config->BusName, Driver->NtClass, Driver->Service);
	return(bResult);
}
