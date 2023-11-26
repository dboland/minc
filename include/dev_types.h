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

/* Device classes (sys/device.h) */

#define DEV_CLASS_DULL		0x0000			/* generic, no special info */
#define DEV_CLASS_SYSTEM	DEV_CLASS_DULL
#define DEV_CLASS_CPU		0x0100
#define DEV_CLASS_DISK		0x0200
#define DEV_CLASS_IFNET		0x0300
#define DEV_CLASS_TAPE		0x0400
#define DEV_CLASS_MEDIA		DEV_CLASS_TAPE
#define DEV_CLASS_TTY		0x0500
#define DEV_CLASS_SERIAL	DEV_CLASS_TTY
#define DEV_CLASS_PRINTER	0x0600
#define DEV_CLASS_DISPLAY	0x0700
#define DEV_CLASS_KEYBOARD	0x0800
#define DEV_CLASS_MOUSE		0x0900
#define DEV_CLASS_STORAGE	0x0A00
#define DEV_CLASS_USB		0x0B00
#define DEV_CLASS_MAX		12

/* Hardware buses */

#define DEV_BUS_MAIN		56
#define DEV_BUS_BIOS		DEV_BUS_MAIN
#define DEV_BUS_CPU		DEV_BUS_MAIN
#define DEV_BUS_UEFI		DEV_BUS_BIOS		/* Unified Extensible Firmware Interface bus */
#define DEV_BUS_APM		DEV_BUS_BIOS		/* Advanced Power Management interface bus */
#define DEV_BUS_ACPI		DEV_BUS_BIOS		/* Advanced Configuration and Power Interface bus */
#define DEV_BUS_PCI		48			/* Peripheral Component Interconnect bus */
#define DEV_BUS_SATA		DEV_BUS_PCI		/* Serial Advanced Technology Attachment bus */
#define DEV_BUS_ISA		40			/* Industry Standard Architecture bus */
#define DEV_BUS_FDC		DEV_BUS_ISA		/* Floppy Disk controller bus */
#define DEV_BUS_UART		32			/* Universal Asynchronous Receiver-Transmitter bus */
#define DEV_BUS_USB		DEV_BUS_UART		/* Universal Serial Bus */
#define DEV_BUS_IDE		24			/* Integrated Disk Electronics bus */
#define DEV_BUS_HDC		DEV_BUS_IDE		/* Hard Disk Controller bus */
#define DEV_BUS_SCSI		16			/* Small Computer System Interface bus  */

/* System device types */

#define DEV_TYPE_SWD		(DEV_CLASS_DULL)
#define DEV_TYPE_ACPI		(DEV_CLASS_DULL + DEV_BUS_ACPI)
#define DEV_TYPE_OHCI		(DEV_CLASS_DULL + DEV_BUS_PCI)	/* USB Open Host Controller Interface */
#define DEV_TYPE_EHCI		(DEV_CLASS_DULL + DEV_BUS_USB)	/* USB Enhanced Host Controller Interface (USB 2.0) */
#define DEV_TYPE_IDE		(DEV_CLASS_DULL + DEV_BUS_IDE)	/* Integrated Disk Electronics device */

#define DEV_TYPE_MEM		(DEV_CLASS_CPU + 1)		/* physical memory device */
#define DEV_TYPE_KMEM		(DEV_CLASS_CPU + 2)		/* kernel memory device (libposix.dll) */
#define DEV_TYPE_NULL		(DEV_CLASS_CPU + 3)		/* null byte write device */
#define DEV_TYPE_PORT		(DEV_CLASS_CPU + 4)		/* I/O port access */
#define DEV_TYPE_ZERO		(DEV_CLASS_CPU + 5)		/* null byte read device */
#define DEV_TYPE_RANDOM		(DEV_CLASS_CPU + 8)		/* random generator pseudo device */
#define DEV_TYPE_URANDOM	(DEV_CLASS_CPU + 9)		/* crypto random generator pseudo device */
#define DEV_TYPE_ROUTE		(DEV_CLASS_CPU + 10)		/* message routing pseudo device */
#define DEV_TYPE_STDIN		(DEV_CLASS_CPU + 16)		/* standard stream pseudo device */
#define DEV_TYPE_STDOUT		(DEV_CLASS_CPU + 17)		/* standard stream pseudo device */
#define DEV_TYPE_STDERR		(DEV_CLASS_CPU + 18)		/* standard stream pseudo device */
#define DEV_TYPE_SWAP		(DEV_CLASS_CPU + 24)		/* disk swap pseudo device */
#define DEV_TYPE_RAMDISK	(DEV_CLASS_CPU + 32)		/* Random Access Memory disk device (rd*) */
#define DEV_TYPE_PROCESSOR	(DEV_CLASS_CPU + DEV_BUS_CPU)	/* Central Processing Unit */

#define DEV_TYPE_AHCI		(DEV_CLASS_DISK + DEV_BUS_SATA)	/* Serial ATA Advanced Host Controller Interface */
#define DEV_TYPE_FDC		(DEV_CLASS_DISK + DEV_BUS_FDC)	/* Floppy Disk Controller */
#define DEV_TYPE_USBSTOR	(DEV_CLASS_DISK + DEV_BUS_USB)	/* USB Mass Storage controller */
#define DEV_TYPE_SD		(DEV_CLASS_DISK + DEV_BUS_SCSI)	/* SCSI Disk controller */
#define DEV_TYPE_WD		(DEV_CLASS_DISK + DEV_BUS_IDE)	/* WD100x compatible hard disk controller */

#define DEV_TYPE_NDIS		(DEV_CLASS_IFNET)
#define DEV_TYPE_TUNNEL		(DEV_CLASS_IFNET + 16)		/* Tunnel type encapsulation */
#define DEV_TYPE_LOOPBACK	(DEV_CLASS_IFNET + 20)		/* loop-back network pseudo device */
#define DEV_TYPE_PPP		(DEV_CLASS_IFNET + 24)		/* Point-To-Point network device */
#define DEV_TYPE_ETH		(DEV_CLASS_IFNET + 28)		/* Ethernet network device */
#define DEV_TYPE_WLAN		(DEV_CLASS_IFNET + 36)		/* IEEE80211 wireless network device */
#define DEV_TYPE_NIC		(DEV_CLASS_IFNET + DEV_BUS_PCI)	/* Network Interface Card */

#define DEV_TYPE_MEDIA		(DEV_CLASS_MEDIA)
#define DEV_TYPE_USBVIDEO	(DEV_CLASS_MEDIA + DEV_BUS_USB)
#define DEV_TYPE_AUDIO		(DEV_CLASS_MEDIA + DEV_BUS_PCI)
#define DEV_TYPE_MAGTAPE	(DEV_CLASS_MEDIA + DEV_BUS_SCSI)

#define DEV_TYPE_CONSOLE	(DEV_CLASS_TTY + 1)		/* system console master device */
#define DEV_TYPE_PTM		(DEV_CLASS_TTY + 2)		/* serial multiplex device */
#define DEV_TYPE_PRINTK		(DEV_CLASS_TTY + 3)		/* boot time output device (printf for kernel) */
#define DEV_TYPE_INPUT		(DEV_CLASS_TTY + 4)		/* interix input multiplex device */
#define DEV_TYPE_SCREEN		(DEV_CLASS_TTY + 5)		/* interix output multiplex device */
#define DEV_TYPE_PTY		(DEV_CLASS_TTY + 8)		/* pseudo serial master device */
#define DEV_TYPE_TTY		(DEV_CLASS_TTY + 24)		/* all serial slave devices */
#define DEV_TYPE_COM		(DEV_CLASS_TTY + DEV_BUS_ISA)	/* EIA RS232 serial master device */

#define DEV_TYPE_QUEUE		(DEV_CLASS_PRINTER)
#define DEV_TYPE_USBPRINT	(DEV_CLASS_PRINTER + DEV_BUS_USB)
#define DEV_TYPE_LPT		(DEV_CLASS_PRINTER + DEV_BUS_ISA)

#define DEV_TYPE_WSDISPLAY	(DEV_CLASS_DISPLAY)
#define DEV_TYPE_VGA		(DEV_CLASS_DISPLAY + DEV_BUS_PCI)

#define DEV_TYPE_HIDKBD		(DEV_CLASS_KEYBOARD)
#define DEV_TYPE_COMKBD		(DEV_CLASS_KEYBOARD + DEV_BUS_CPU)
#define DEV_TYPE_WSKBD		(DEV_CLASS_KEYBOARD + DEV_BUS_PCI)

#define DEV_TYPE_HIDMOUSE	(DEV_CLASS_MOUSE)
#define DEV_TYPE_COMMOUSE	(DEV_CLASS_MOUSE + DEV_BUS_CPU)
#define DEV_TYPE_WSMOUSE	(DEV_CLASS_MOUSE + DEV_BUS_PCI)

#define DEV_TYPE_VOLUME		(DEV_CLASS_STORAGE)
#define DEV_TYPE_REMOTE		(DEV_CLASS_STORAGE + DEV_BUS_MAIN)	/* Server Message Block storage */
#define DEV_TYPE_FLOPPY		(DEV_CLASS_STORAGE + DEV_BUS_FDC)	/* Floppy Disk storage */
#define DEV_TYPE_CDROM		(DEV_CLASS_STORAGE + DEV_BUS_SCSI)	/* CDROM storage */
#define DEV_TYPE_ROOT		(DEV_CLASS_STORAGE + DEV_BUS_HDC)	/* Root mount point */

#define DEV_TYPE_HID		(DEV_CLASS_USB)
#define DEV_TYPE_USBHUB		(DEV_CLASS_USB + DEV_BUS_USB)
#define DEV_TYPE_UHCI		(DEV_CLASS_USB + DEV_BUS_PCI)	/* USB Universal Host Controller Interface */

