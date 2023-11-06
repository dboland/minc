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

//#define WIN_MAILSLOT_ROOT	L"\\\\.\\MAILSLOT\\"
//#define WIN_PIPE_ROOT		L"\\\\.\\PIPE\\"
#define WIN_PROCESS_ROOT	L"\\\\.\\GLOBALROOT\\"
#define WIN_VOLUME_ROOT		L"\\\\.\\"
#define WIN_DEVICE_ROOT		L"\\??\\"

typedef enum _WIN_FS_TYPE {
	FS_TYPE_UNKNOWN,
	FS_TYPE_DISK,
	FS_TYPE_CHAR,
	FS_TYPE_PIPE,
	FS_TYPE_MAILSLOT,
	FS_TYPE_DEVICE,
	FS_TYPE_DRIVE,
	FS_TYPE_PROCESS,
	FS_TYPE_WINSOCK,
	FS_TYPE_REGISTRY,
	FS_TYPE_VOLUME,
	FS_TYPE_NPF,
	FS_TYPE_LINK,
	FS_TYPE_NDIS,
	FS_TYPE_MAX
} WIN_FS_TYPE;

typedef enum _WIN_VTYPE {
	WIN_VNON,
	WIN_VREG,
	WIN_VDIR,
	WIN_VBLK,
	WIN_VCHR,
	WIN_VLNK,
	WIN_VSOCK,
	WIN_VFIFO,
	WIN_VBAD,
	WIN_VMAX
} WIN_VTYPE;

typedef enum _WIN_VTAGTYPE {
	WIN_VT_NON,
	WIN_VT_UFS,
	WIN_VT_NFS,
	WIN_VT_MFS,
	WIN_VT_MSDOSFS,
	WIN_VT_PORTAL,
	WIN_VT_PROCFS,
	WIN_VT_AFS,
	WIN_VT_ISOFS,
	WIN_VT_ADOSFS,
	WIN_VT_EXT2FS,
	WIN_VT_VFS,
	WIN_VT_NTFS,
	WIN_VT_UDF,
	WIN_VT_FUSEFS,
	WIN_VT_TMPFS
} WIN_VTAGTYPE;

#define WIN_DRIVE_MAX		26
#define WIN_UNIT_MAX		64

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
#define DEV_BUS_UEFI		DEV_BUS_BIOS		/* Unified Extensible Firmware Interface */
#define DEV_BUS_APM		DEV_BUS_BIOS		/* Advanced Power Management interface */
#define DEV_BUS_ACPI		DEV_BUS_BIOS		/* Advanced Configuration and Power Interface */
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
//#define DEV_TYPE_PCI		(DEV_CLASS_DULL + DEV_BUS_PCI)  /* Peripheral Component Interconnect */
#define DEV_TYPE_OHCI		(DEV_CLASS_DULL + DEV_BUS_PCI)	/* USB Open Host Controller Interface */
#define DEV_TYPE_EHCI		(DEV_CLASS_DULL + DEV_BUS_USB)	/* USB Enhanced Host Controller Interface (USB 2.0) */
#define DEV_TYPE_IDE		(DEV_CLASS_DULL + DEV_BUS_IDE)	/* Integrated Disk Electronics device */
//#define DEV_TYPE_ROOT		(DEV_CLASS_DULL + DEV_BUS_HDC)

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
#define DEV_TYPE_SWAP		(DEV_CLASS_CPU + 24)		/* sisk swap pseudo device */
#define DEV_TYPE_RAMDISK	(DEV_CLASS_CPU + 32)		/* Random Access Memory disk device (rd*) */
#define DEV_TYPE_PROCESSOR	(DEV_CLASS_CPU + DEV_BUS_CPU)	/* Central Processing Unit */

#define DEV_TYPE_AHCI		(DEV_CLASS_DISK + DEV_BUS_SATA)	/* Serial ATA Advanced Host Controller Interface */
#define DEV_TYPE_FDC		(DEV_CLASS_DISK + DEV_BUS_FDC)	/* Floppy Disk Controller */
#define DEV_TYPE_USB		(DEV_CLASS_DISK + DEV_BUS_USB)	/* Universal Serial Bus Controller */
#define DEV_TYPE_SD		(DEV_CLASS_DISK + DEV_BUS_SCSI)	/* SCSI disk driver */
#define DEV_TYPE_WD		(DEV_CLASS_DISK + DEV_BUS_IDE)

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

#define DEV_TYPE_CONSOLE	(DEV_CLASS_TTY + 1)		/* system console master device */
#define DEV_TYPE_PTM		(DEV_CLASS_TTY + 2)		/* serial multiplex device */
#define DEV_TYPE_PRINTK		(DEV_CLASS_TTY + 3)		/* boot time output device (printf for kernel) */
#define DEV_TYPE_INPUT		(DEV_CLASS_TTY + 4)		/* interix input multiplex device */
#define DEV_TYPE_SCREEN		(DEV_CLASS_TTY + 5)		/* interix output multiplex device */
#define DEV_TYPE_PTY		(DEV_CLASS_TTY + 8)		/* pseudo serial master device */
#define DEV_TYPE_TTY		(DEV_CLASS_TTY + 24)		/* all serial slave devices */
#define DEV_TYPE_COM		(DEV_CLASS_TTY + DEV_BUS_ISA)	/* EIA RS232 serial master device */

#define DEV_TYPE_QUEUE		(DEV_CLASS_PRINTER)
#define DEV_TYPE_USBLPT		(DEV_CLASS_PRINTER + DEV_BUS_USB)
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
#define DEV_TYPE_USBMASS	(DEV_CLASS_STORAGE + DEV_BUS_USB)	/* Serial disk storage */
#define DEV_TYPE_CDROM		(DEV_CLASS_STORAGE + DEV_BUS_SCSI)	/* CDROM storage */
#define DEV_TYPE_ROOT		(DEV_CLASS_STORAGE + DEV_BUS_HDC)	/* Root mount point */

#define DEV_TYPE_HID		(DEV_CLASS_USB)
#define DEV_TYPE_USBHUB		(DEV_CLASS_USB + DEV_BUS_USB)
#define DEV_TYPE_UHCI		(DEV_CLASS_USB + DEV_BUS_PCI)	/* USB Universal Host Controller Interface */

/* sys/ttycom.h */

#define WIN_TIOCFLUSH	16
#define WIN_TIOCSETA	20
#define WIN_TIOCSETAW	21
#define WIN_TIOCSETAF	22
#define WIN_TIOCGETD	26
#define WIN_TIOCSFLAGS	92
#define WIN_TIOCGFLAGS	93
#define WIN_TIOCDRAIN	94
#define WIN_TIOCSCTTY	97
#define WIN_TIOCGSID	99
#define WIN_TIOCSWINSZ	103
#define WIN_TIOCGWINSZ	104
#define WIN_TIOCSPGRP	118
#define WIN_TIOCGPGRP	119

/* sys/termios.h */

#define WIN_ISIG		0x00800000

/* line In */

#define WIN_IXON		0x02000000
#define WIN_IXOFF		0x04000000
#define WIN_INLCR		0x00400000	/* Ye Olde TTY had separate key for CR */
#define WIN_ICRNL		0x01000000

/* line Out */

#define WIN_ONLCR		0x00020000
#define WIN_OXTABS		0x00040000
#define WIN_OCRNL		0x00100000

/* sys/syslimits.h */

#define WIN_MAX_INPUT	128
#define WIN_TTY_MAX	WIN_UNIT_MAX
#define WIN_NAME_MAX	16

/* wincon.h */

#define ENABLE_ALL_INPUT		(WIN_INLCR | WIN_ICRNL | ENABLE_PROCESSED_INPUT | ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT)
#define ENABLE_ALL_OUTPUT		(WIN_ONLCR | WIN_OXTABS)

#define KeyEvent			Event.KeyEvent
#define MouseEvent			Event.MouseEvent
#define WindowBufferSizeEvent		Event.WindowBufferSizeEvent
#define MenuEvent			Event.MenuEvent
#define FocusEvent			Event.FocusEvent

#define ENABLE_VIRTUAL_TERMINAL_INPUT		0x0200

#define ENABLE_VIRTUAL_TERMINAL_PROCESSING	0x0004
#define ENABLE_LVB_GRID_WORLDWIDE		0x0010
#define DISABLE_NEWLINE_AUTO_RETURN		0x0008

/* 
 * vfs_namei.c
 */

#define TypeNameLink			0x6B6E6C2E	/* ".lnk" */
#define TypeNameExe			0x6578652E	/* ".exe" */
#define TypeNameVirtual			0x7366762E	/* ".vfs" */

#define FILE_ATTRIBUTE_SYMLINK		0x00000008
#define FILE_ATTRIBUTE_TRAVERSE		(FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)
#define FILE_ATTRIBUTE_MOUNT		(FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_SYSTEM)
#define FILE_ATTRIBUTE_VFS		(FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DIRECTORY)

typedef struct _WIN_INODE {
	DWORD Magic;			/* file signature/mount identifier */
	WIN_VTYPE FileType;		/* in userland, everything is a file */
	DWORD DeviceType;		/* in kernelland, everything is a device */
	DWORD DeviceId;			/* device inode is on */
	WIN_FS_TYPE FSType;		/* file system Handle is from */
	WCHAR Name[WIN_PATH_MAX];
} WIN_INODE;

typedef struct _WIN_NAMEIDATA {
	DWORD MountId;
	WIN_VTYPE FileType;
	DWORD DeviceType;
	DWORD DeviceId;
	WIN_FS_TYPE FSType;
	WCHAR Resolved[WIN_PATH_MAX];
	DWORD Attribs;
	DWORD Flags;			/* see below */
	WCHAR *Last;
	WCHAR *R;			/* current WCHAR in resolved path buffer */
	WCHAR *S;			/* current WCHAR in source path buffer */
} WIN_NAMEIDATA;

#define WIN_NAMEI	WIN_NAMEIDATA
#define WIN_SYMLOOP_MAX		32

#define WIN_LOCKLEAF		0x000004	/* lock inode on return */
#define WIN_LOCKPARENT		0x000008	/* want parent vnode returned locked */
#define WIN_WANTPARENT		0x000010	/* want parent vnode returned unlocked */
#define WIN_NOCACHE		0x000020	/* name must not be left in cache */
#define WIN_FOLLOW		0x000040	/* follow symbolic links */
#define WIN_NOFOLLOW		0x000000	/* do not follow symbolic links (pseudo) */
#define WIN_MODMASK		0x0000fc	/* mask of operational modifiers */
#define WIN_NOCROSSMOUNT	0x000100	/* do not cross mount points */
#define WIN_RDONLY		0x000200	/* lookup with read-only semantics */
#define WIN_HASBUF		0x000400	/* has allocated pathname buffer */
#define WIN_SAVENAME		0x000800	/* save pathname buffer */
#define WIN_SAVESTART		0x001000	/* save starting directory */
#define WIN_ISDOTDOT		0x002000	/* current component name is .. */
#define WIN_MAKEENTRY		0x004000	/* entry is to be added to name cache */
#define WIN_ISLASTCN		0x008000	/* this is last component of pathname */
#define WIN_ISSYMLINK		0x010000	/* symlink needs interpretation */
#define WIN_REQUIREDIR		0x080000	/* must be a directory */
#define WIN_STRIPSLASHES	0x100000	/* strip trailing slashes */
#define WIN_PDIRUNLOCK		0x200000	/* vfs_lookup() unlocked parent dir */

/*
 * vfs_device.c
 */

typedef struct _WIN_DEVICE {
	DWORD Magic;
	WIN_VTYPE FileType;
	DWORD DeviceType;
	DWORD DeviceId;
	WIN_FS_TYPE FSType;		/* file system Handles are in */
	CHAR Name[MAX_NAME];
	HANDLE Handle;
	HANDLE Input;
	HANDLE Output;
	DWORD Flags;			/* see below */
	DWORD Index;
	WCHAR NtName[MAX_NAME];
	WCHAR ClassId[MAX_GUID];
} WIN_DEVICE;

#define WIN_DVF_BUS_READY		0x0100
#define WIN_DVF_PORT_READY		0x0200
#define WIN_DVF_DRIVER_READY		0x0400

/* Flags from NetBSD */

#define WIN_DVF_ACTIVE			0x0001		/* device is activated */
#define WIN_DVF_PRIV_ALLOC		0x0002		/* device private storage != device */
#define WIN_DVF_POWER_HANDLERS		0x0004		/* device has suspend/resume support */
#define WIN_DVF_CLASS_SUSPENDED		0x0008		/* device class suspend was called */
#define WIN_DVF_DRIVER_SUSPENDED	0x0010		/* device driver suspend was called */
#define WIN_DVF_BUS_SUSPENDED		0x0020		/* device bus suspend was called */
#define WIN_DVF_ATTACH_INPROGRESS	0x0040		/* device attach is in progress */
#define WIN_DVF_DETACH_SHUTDOWN		0x0080		/* device detaches safely at shutdown */

typedef struct _WIN_CFDRIVER {
	DWORD Flags;
	DWORD DeviceType;
	CHAR Name[MAX_NAME];
	WCHAR ClassId[MAX_GUID];
	WCHAR NtClass[MAX_NAME];
	WCHAR Service[MAX_NAME];
	WCHAR Location[MAX_COMMENT];
	WCHAR Comment[MAX_COMMENT];
} WIN_CFDRIVER;

typedef struct _WIN_CFDATA {
	LPWSTR Strings;
	LPCWSTR Next;
	DWORD FSType;
	LPCWSTR DosPath;
	LPCWSTR NtName;
	DWORD Depth;
	WCHAR BusName[MAX_NAME];
	WCHAR ClassName[MAX_NAME];
	WCHAR NtPath[WIN_PATH_MAX];
} WIN_CFDATA;

typedef WIN_DEVICE WIN_DEV_CLASS[WIN_UNIT_MAX];

#define DEVICE(rid)	(&__Devices[rid >> 8][rid & 0xFF])

/*
 * vfs_fcntl.c
 */

#define WIN_OPEN_MAX		64

#define WIN_F_DUPFD		0
#define WIN_F_GETOWN		5
#define WIN_F_SETOWN		6
#define WIN_F_DUPFD_CLOEXEC	10

#define WIN_FSIZE_MAX		(DWORDLONG)((MAX_FILESIZE + WIN_S_BLKSIZE - 1) / WIN_S_BLKSIZE)

typedef struct _WIN_FLAGS {
	ACCESS_MASK Access;	/* Desired access */
	DWORD Share;		/* Share mode */
	DWORD Creation;		/* Creation distribution */
	DWORD Attribs;		/* Flags (high) and Attibutes (low) */
	BOOL CloseExec;
} WIN_FLAGS;

typedef struct _WIN_VNODE {
	DWORD MountId;
	WIN_VTYPE FileType;
	DWORD DeviceType;
	DWORD DeviceId;
	WIN_FS_TYPE FSType;
	DWORD FileId;
	ACCESS_MASK Access;
	DWORD Attribs;
	HANDLE Event;
	ULONG Pending;		/* pending events */
	DWORD Flags;		/* handle information */
	union {
		HANDLE Handle;
		UINT Socket;
		HKEY Key;
	};
	WIN_DEVICE *Device;
	HANDLE Lock;
	HANDLE Object;
	DWORD Index;
	BOOL CloseExec;
	LONG Owner;
} WIN_VNODE;

#define LOCKFILE_SHARED		0x00000000
#define LOCKFILE_UNLOCK		0x00000010

/*
 * vfs_dirent.c
 */

typedef struct _WIN_DIRENT {
	WIN_VTYPE FileType;
	DWORD Index;
	WCHAR FileName[MAX_PATH];
} WIN_DIRENT;

/*
 * vfs_proc.c
 */

#define WIN_MAX_PROCTITLE	MAX_PATH

typedef struct _WIN_PSTRING {
	WCHAR Command[WIN_MAX_PROCTITLE];
	CHAR Message[MAX_MESSAGE];	/* not used so far */
} WIN_PSTRING;

#define MESSAGE(Task)	__Strings[Task->TaskId].Message

/*
 * vfs_mount.c
 */

#define WIN_MOUNT_MAX	(WIN_DRIVE_MAX + 1)

#define MOUNTID(ch)	(DWORD)(1 + msvc_tolower(ch) - 'a')

typedef struct _WIN_MOUNT {
	DWORD MountId;
	WIN_VTYPE FileType;
	DWORD DeviceType;
	DWORD DeviceId;
	WIN_FS_TYPE FSType;
	WCHAR Path[MAX_PATH];
	DWORD VolumeSerial;
	DWORD Flags;
	FILETIME Time;
	UINT DriveType;
	WCHAR Drive[MAX_LABEL];
	WCHAR NtName[MAX_NAME];
} WIN_MOUNT;

typedef struct _WIN_STATFS {
	DWORD Serial;
	DWORD MaxPath;
	DWORD Flags;
	DWORD DeviceId;
	DWORD SectorsPerCluster;
	DWORD BytesPerSector;
	DWORD FreeClusters;
	DWORD ClustersTotal;
	FILETIME MountTime;
	WCHAR TypeName[MAX_LABEL];
	WCHAR Label[MAX_LABEL];
	WCHAR Drive[MAX_LABEL];
} WIN_STATFS;

/* Control flags for getfsstat() */

#define WIN_MNT_UPDATE		0x00010000	/* not a real mount, just an update */
#define WIN_MNT_DELEXPORT	0x00020000	/* delete export host lists */
#define WIN_MNT_RELOAD		0x00040000	/* reload filesystem data */
#define WIN_MNT_FORCE		0x00080000	/* force unmount or readonly change */
#define WIN_MNT_NOWAIT		0x00100000
#define WIN_MNT_REVERSED	0x00200000	/* load reversed file system table */
#define WIN_MNT_DEBUG		0x00400000
#define WIN_MNT_WANTRDWR	0x02000000	/* want upgrade to read/write */
#define WIN_MNT_SOFTDEP		0x04000000	/* soft dependencies being done */
#define WIN_MNT_DOOMED		0x08000000	/* device behind filesystem is gone */

/*
 * vfs_stat.c
 */

typedef struct _WIN_VATTR {
	DWORD Attributes;
	FILETIME CreationTime;
	FILETIME LastAccessTime;
	FILETIME LastWriteTime;
	DWORD VolumeSerialNumber;
	DWORD FileSizeHigh;
	DWORD FileSizeLow;
	DWORD NumberOfLinks;
	DWORD FileIndexHigh;		/* object identifier high */
	DWORD FileIndexLow;		/* object identifier low */
	DWORD DeviceId;			/* device inode is on */
	DWORD SpecialId;		/* device inode represents (if special file) */
	CHAR VolumeLabel[MAX_LABEL];	/* vfs_symlink() */
	WIN_MODE Mode;
	SID8 UserSid;
	SID8 GroupSid;
	DWORD BlockSize;
} WIN_VATTR;

/*
 * vfs_time.c
 */

#define WIN_CLOCK_REALTIME		0
#define WIN_CLOCK_VIRTUAL		1
#define WIN_CLOCK_PROCESS_CPUTIME_ID	2
#define WIN_CLOCK_MONOTONIC		3
#define WIN_CLOCK_THREAD_CPUTIME_ID	4
#define WIN_CLOCK_UPTIME		5

/*
 * vfs_sysctl.c
 */

typedef enum _WIN_GLOBAL {
	WIN_KERN_SECURELVL,
	WIN_KERN_VERSION,
	WIN_KERN_BUILD,
	WIN_KERN_CLOCKRATE,
	WIN_HW_PAGESIZE,
	WIN_KERN_MSGBUF,
	WIN_GLOBAL_MAX
} WIN_GLOBAL;

typedef struct _WIN_IFENUM {
	DWORD Ethernet;
	DWORD PPP;
	DWORD Loopback;
	DWORD WLan;
	DWORD Other;
} WIN_IFENUM;

/*
 * vfs_sched.c
 */

typedef struct _WIN_TASK {
	HANDLE Handle;
	DWORD ThreadId;
	DWORD ProcessId;
	DWORD Status;			/* exit status */
	DWORD TaskId;
	DWORD ParentId;
	DWORD GroupId;
	DWORD SessionId;
	DWORD TerminalId;
	DWORD Nice;
	DWORD Flags;
	DWORD Depth;
	DWORDLONG Ticks;		/* number of ticks in nanoseconds */
	LONG Interval;			/* last interval in milliseconds */
	SID8 UserSid;			/* effective user id */
	SID8 GroupSid;			/* effective group id */
	HANDLE TraceHandle;
	DWORD TracePoints;
	FILETIME Started;
	DWORD State;
	DWORD Code;			/* syscall currently running */
	HANDLE Timer;
	UINT ProcMask;			/* blocked signals */
	UINT Pending;			/* pending signals */
	INT Error;
	UINT FileMask;
	BOOL IsSetUGid;
	UINT RealUid, SavedUid;
	UINT RealGid, SavedGid;
	WIN_INODE Path;				/* current working directory */
	DWORDLONG Limit[WIN_RLIM_NLIMITS];
	WIN_ATEXITPROC AtExit[WIN_ATEXIT_MAX];
	WIN_SIGACTION Action[WIN_NSIG];
	WIN_VNODE Node[WIN_OPEN_MAX];
} WIN_TASK;

/*
 * vfs_syscall.c
 */

#define WIN_SYS_exit	1
#define WIN_SYS_close	6
#define WIN_SYS_kill	37

/*
 * vfs_poll.c
 */

typedef struct _WIN_POLLFD {
	INT FileId;
	SHORT Events;
	SHORT Result;
} WIN_POLLFD;

#define WIN_POLLIN		0x0001
#define WIN_POLLPRI		0x0002
#define WIN_POLLOUT		0x0004
#define WIN_POLLERR		0x0008
#define WIN_POLLHUP		0x0010
#define WIN_POLLNVAL		0x0020
#define WIN_POLLRDNORM		0x0040
#define WIN_POLLRDBAND		0x0080
#define WIN_POLLWRBAND		0x0100
#define WIN_POLLIGNORE		(WIN_POLLERR | WIN_POLLHUP | WIN_POLLNVAL)

/*
 * vfs_termio.c
 */

typedef struct _WIN_WINSIZE {
	USHORT Row;
	USHORT Column;
	USHORT XPixel;
	USHORT YPixel;
} WIN_WINSIZE;

typedef struct _WIN_TERMIO {
	DWORD TerminalId;
	DWORD DeviceId;
	DWORD GroupId;
	DWORD SessionId;
	WIN_WINSIZE WinSize;
	DWORD Mode[2];
	BOOL RVideo;
	DWORD ScrollRate;
	BOOL NoWait;
	DWORD AltFont;
	BOOL VEdit;
	COORD Cursor;
	DWORD Flags;
} WIN_TERMIO;

typedef struct _WIN_PTMGET {
	WIN_VNODE Master;
	WIN_VNODE Slave;
} WIN_PTMGET;

/*
 * vfs.c
 */

typedef struct _WIN_SESSION {
	WIN_TASK Tasks[WIN_CHILD_MAX];
	WIN_DEV_CLASS Devices[DEV_CLASS_MAX];
	WIN_TERMIO Terminals[WIN_TTY_MAX];
	WIN_PSTRING Strings[WIN_CHILD_MAX];
	WIN_MOUNT Mounts[WIN_MOUNT_MAX];
	LARGE_INTEGER Globals[WIN_GLOBAL_MAX];
} WIN_SESSION;

/*
 * pipe.c
 */

#define WIN_PIPE_BUF	1024

/*
 * registry.c
 */

/* Registry file types */

#define REG_DRIVER	1
#define REG_CLASS	2
#define REG_IFACE	3
#define REG_MOUNT	4
#define REG_TTY		5
