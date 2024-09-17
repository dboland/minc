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

typedef enum _WIN_FS_TYPE {
	FS_TYPE_UNKNOWN,
	FS_TYPE_DISK,
	FS_TYPE_CHAR,
	FS_TYPE_PIPE,
	FS_TYPE_MAILSLOT,
	FS_TYPE_PDO,
	FS_TYPE_DRIVE,
	FS_TYPE_PROCESS,
	FS_TYPE_WINSOCK,
	FS_TYPE_REGISTRY,
	FS_TYPE_VOLUME,
	FS_TYPE_NPF,
	FS_TYPE_LINK,
	FS_TYPE_NDIS,
	FS_TYPE_SHELL,
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

/* sys/syslimits.h */

#define WIN_NAME_MAX		16
#define WIN_PATH_MAX		1024
#define WIN_PIPE_BUF		1024

/*
 * vfs_device.c
 */

#define WIN_UNIT_MAX		64

typedef struct _WIN_DEVICE {
	WIN_VTYPE FileType;
	DWORD DeviceType;
	DWORD DeviceId;
	WIN_FS_TYPE FSType;		/* file system Handles are from */
	CHAR Name[MAX_NAME];
	HANDLE Event;
	HANDLE Input;
	HANDLE Output;
	DWORD Flags;			/* see below */
	DWORD Index;			/* driver-specific software context */
	WCHAR NtName[MAX_NAME];
	WCHAR ClassId[MAX_GUID];
	WCHAR NtPath[MAX_PATH];
} WIN_DEVICE;

#define WIN_DVF_PDO_READY		0x0100
#define WIN_DVF_DRIVE_READY		0x0200
#define WIN_DVF_CONFIG_READY		0x0400

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
	DWORD DeviceId;
	CHAR Name[MAX_NAME];
	WCHAR ClassId[MAX_GUID];
	WCHAR NtClass[MAX_NAME];
	WCHAR Service[MAX_NAME];
	WCHAR Location[MAX_COMMENT];
	WCHAR Comment[MAX_COMMENT];
	WCHAR NtPath[MAX_PATH];
} WIN_CFDRIVER;

typedef struct _WIN_CFDATA {
	LPWSTR Strings;
	LPCWSTR Next;
	DWORD FSType;
	DWORD DeviceType;
	LPCWSTR DosPath;
	LPCWSTR NtName;
	DWORD Depth;
	WCHAR BusName[MAX_NAME];
	WCHAR ClassName[MAX_NAME];
	WCHAR NtPath[MAX_TEXT];
} WIN_CFDATA;

typedef WIN_DEVICE WIN_DEV_CLASS[WIN_UNIT_MAX];

#define DEVICE(rid)		(&__Devices[rid >> 8][rid & 0xFF])

/* 
 * vfs_namei.c
 */

#define TypeNameLink		0x6B6E6C2E	/* ".lnk" */
#define TypeNameExe		0x6578652E	/* ".exe" */
#define TypeNameVirtual		0x7366762E	/* ".vfs" */

#define FILE_ATTRIBUTE_LABEL	0x00000008

#define FILE_CLASS_INODE	(FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_SYSTEM)
#define FILE_CLASS_MOUNT	(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_SYSTEM)
#define FILE_CLASS_ROOT		(FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_SYSTEM)

#define WIN_SYMLOOP_MAX		8

typedef struct _WIN_INODE {
	DWORD Magic;		/* file signature */
	DWORD DeviceId;		/* in kernelland, everything is a device */
	WIN_VTYPE FileType;	/* in userland, everything is a file */
	WIN_FS_TYPE FSType;
	DWORD NameSize;
	DWORD Index;
	PVOID Object;
} WIN_INODE;

#define INAMESIZE(wcs)		((win_wcslen(wcs) + 1) * sizeof(WCHAR))

typedef struct _WIN_NAMEIDATA {
	DWORD MountId;
	DWORD DeviceId;
	WIN_VTYPE FileType;
	WIN_FS_TYPE FSType;
	DWORD Size;
	DWORD Index;
	HANDLE Object;
	DWORD Attribs;
	DWORD Flags;			/* see below */
	WCHAR *Base;
	WCHAR *Last;
	WCHAR *R;			/* current WCHAR in resolved path buffer */
	WCHAR *S;			/* current WCHAR in source path buffer */
	WCHAR Resolved[WIN_PATH_MAX];
} WIN_NAMEIDATA;

#define WIN_PATHCOPY		0x00400000
#define WIN_REQUIREOBJECT	0x01000000

/* sys/namei.h */

#define WIN_LOCKLEAF		0x00000004	/* lock inode on return */
#define WIN_LOCKPARENT		0x00000008	/* want parent vnode returned locked */
#define WIN_WANTPARENT		0x00000010	/* want parent vnode returned unlocked */
#define WIN_NOCACHE		0x00000020	/* name must not be left in cache */
#define WIN_FOLLOW		0x00000040	/* follow symbolic links */
#define WIN_NOFOLLOW		0x00000000	/* do not follow symbolic links (pseudo) */
#define WIN_MODMASK		0x000000fc	/* mask of operational modifiers */

#define WIN_NOCROSSMOUNT	0x00000100	/* do not cross mount points */
#define WIN_RDONLY		0x00000200	/* lookup with read-only semantics */
#define WIN_HASBUF		0x00000400	/* has allocated pathname buffer */
#define WIN_SAVENAME		0x00000800	/* save pathname buffer */
#define WIN_SAVESTART		0x00001000	/* save starting directory */
#define WIN_ISDOTDOT		0x00002000	/* current component name is .. */
#define WIN_MAKEENTRY		0x00004000	/* entry is to be added to name cache */
#define WIN_ISLASTCN		0x00008000	/* this is last component of pathname */
#define WIN_ISSYMLINK		0x00010000	/* symlink needs interpretation */
#define WIN_REQUIREDIR		0x00080000	/* must be a directory */
#define WIN_STRIPSLASHES	0x00100000	/* strip trailing slashes */
#define WIN_PDIRUNLOCK		0x00200000	/* vfs_lookup() unlocked parent dir */

/*
 * vfs_dirent.c
 */

#define WIN_MAXNAMLEN		255

typedef struct _WIN_DIRENT {
	DWORDLONG FileId;
	DWORDLONG Offset;
	USHORT RecSize;
	UCHAR FileType;
	UCHAR NameSize;
	UCHAR Padding[4];
	CHAR FileName[WIN_MAXNAMLEN + 1];
} WIN_DIRENT;

#define WIN_DT_UNKNOWN		0
#define WIN_DT_FIFO		1
#define WIN_DT_CHR		2
#define WIN_DT_DIR		4
#define WIN_DT_BLK		6
#define WIN_DT_REG		8
#define WIN_DT_LNK		10
#define WIN_DT_SOCK		12

/*
 * vfs_fcntl.c
 */

#define WIN_OPEN_MAX		64
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
	DWORDLONG LockRegion;
	DWORDLONG LockSize;
	HANDLE Object;
	DWORD Index;
	BOOL CloseExec;
	LONG Owner;
} WIN_VNODE;

#define LOCKFILE_SHARED		0x00000000
#define LOCKFILE_UNLOCK		0x00000010

/*
 * vfs_proc.c
 */

#define WIN_MAX_PROCTITLE	MAX_PATH

typedef struct _WIN_PSTRING {
	WCHAR Command[WIN_MAX_PROCTITLE];
	WCHAR Path[WIN_PATH_MAX];
} WIN_PSTRING;

/*
 * vfs_mount.c
 */

#define WIN_DRIVE_MAX	26
#define WIN_MOUNT_MAX	(WIN_DRIVE_MAX + 1)

typedef struct _WIN_MOUNT {
	DWORD MountId;
	DWORD DeviceType;
	DWORD DeviceId;
	DWORD Serial;
	DWORD MaxPath;
	DWORD Flags;
	FILETIME Time;
	WCHAR Volume[MAX_LABEL];
	WCHAR Label[MAX_LABEL];
	WCHAR TypeName[MAX_LABEL];
	WCHAR Path[MAX_PATH];
} WIN_MOUNT;

typedef struct _WIN_STATFS {
	DWORD MaxPath;
	DWORD Flags;
	DWORD DeviceId;
	DWORD SectorsPerCluster;
	DWORD BytesPerSector;
	DWORD FreeClusters;
	DWORD ClustersTotal;
	FILETIME MountTime;
	WCHAR TypeName[MAX_LABEL];
	WCHAR Path[MAX_PATH];
} WIN_STATFS;

#define FILE_VOLUME_MNT_DOOMED	0x00000800
#define FILE_VOLUME_MNT_ROOTFS	0x80000000

/* waitfor flags to vfs_sync() and getfsstat() */

#define WIN_MNT_WAIT        1       /* synchronously wait for I/O to complete */
#define WIN_MNT_NOWAIT      2       /* start all I/O, but do not wait for it */
#define WIN_MNT_LAZY        3       /* push data not written by filesystem syncer */

#define MOUNTID(ch)	(DWORD)(ch ? 1 + msvc_tolower(ch) - 'a' : 0)

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
	WIN_MODE Mode;
	SID8 UserSid;
	SID8 GroupSid;
} WIN_VATTR;

/*
 * vfs_sysctl.c
 */

typedef struct _WIN_GLOBALS {
	UINT SecureLevel;
	CHAR *MsgBuffer;
	UINT MsgBufSize;
	SID8 SidMachine;
	SID8 SidNone;
} WIN_GLOBALS;

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
	DWORD CTTY;
	DWORD MountId;			/* where CWD is mounted */
	DWORD Processor;
	DWORD Nice;
	DWORD Flags;
	DWORD Depth;
	SID8 UserSid;			/* effective user id */
	SID8 GroupSid;			/* effective group id */
	HANDLE TraceHandle;
	DWORD TracePoints;
	FILETIME Started;
	DWORD State;
	DWORD Code;			/* syscall currently running */
	HANDLE Timer;
	DWORDLONG Ticks;		/* number of ticks in nanoseconds */
	LONG Interval;			/* last interval in milliseconds */
	UINT ProcMask;			/* blocked signals */
	UINT Pending;			/* pending signals */
	INT Error;
	UINT FileMask;
	BOOL IsSetUGid;
	UINT RealUid, SavedUid;
	UINT RealGid, SavedGid;
	DWORDLONG Limit[WIN_RLIM_NLIMITS];
	WIN_ATEXITPROC AtExit[WIN_ATEXIT_MAX];
	WIN_SIGACTION Action[WIN_NSIG];
	WIN_VNODE Node[WIN_OPEN_MAX];
} WIN_TASK;

/*
 * vfs_poll.c
 */

typedef struct _WIN_POLLFD {
	INT FileId;
	SHORT Events;
	SHORT Result;
} WIN_POLLFD;

#define WIN_POLLIN		0x0001
#define WIN_POLLPRI		0x0002		/* Pty slave state change detected */
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

#define WIN_TTY_MAX		WIN_UNIT_MAX
#define WIN_NCCS		20

typedef struct _WIN_WINSIZE {
	USHORT Row;
	USHORT Column;
	USHORT XPixel;
	USHORT YPixel;
} WIN_WINSIZE;

typedef struct _WIN_TERMIO {
	UINT IFlags;
	UINT OFlags;
	UINT CFlags;
	UINT LFlags;
	UCHAR Control[WIN_NCCS];
	UINT ISpeed;
	UINT OSpeed;
} WIN_TERMIO;

typedef struct _WIN_TTY {
	DWORD Index;
	DWORD DeviceType;
	DWORD DeviceId;
	DWORD GroupId;
	DWORD SessionId;
	WIN_WINSIZE WinSize;
	WIN_TERMIO Attribs;
	DWORD ScrollRate;
	BOOL VEdit;
	COORD Cursor;
	CONSOLE_SCREEN_BUFFER_INFO Info;
	DWORD Flags;
	DWORD Offset;				/* offset into output buffer */
	CHAR Name[MAX_NAME];
	CHAR Buffer[WIN_MAX_INPUT + 1];
} WIN_TTY;

#define COMMON_LVB_LEADING_BYTE		0x0100
#define COMMON_LVB_TRAILING_BYTE	0x0200
#define COMMON_LVB_GRID_HORIZONTAL	0x0400
#define COMMON_LVB_GRID_LVERTICAL	0x0800
#define COMMON_LVB_GRID_RVERTICAL	0x1000
#define COMMON_LVB_AUTOWRAP		0x2000
#define COMMON_LVB_REVERSE_VIDEO	0x4000
#define COMMON_LVB_UNDERSCORE		0x8000

/* sys/termios.h */

/* Local */

#define WIN_ECHO		0x00000008
#define WIN_ISIG		0x00000080
#define WIN_ICANON		0x00000100

/* line In */

#define WIN_IXON		0x00000200
#define WIN_IXOFF		0x00000400
#define WIN_INLCR		0x00000040	/* Ye Olde TTY had separate key for CR */
#define WIN_ICRNL		0x00000100

/* line Out */

#define WIN_OPOST		0x00000001
#define WIN_ONLCR		0x00000002
#define WIN_OXTABS		0x00000004
#define WIN_OCRNL		0x00000010

/* sys/ttycom.h */

#define TIOCFLAG_ACTIVE		0x00010000

/*
 * vfs_ktrace.c
 */

#define WIN_KTRFAC_ROOT		0x80000000	/* root set this trace */
#define WIN_KTRFAC_INHERIT	0x40000000	/* pass trace flags to children */

/*
 * vfs.c
 */

typedef struct _WIN_SESSION {
	WIN_TASK Tasks[WIN_CHILD_MAX];
	WIN_DEV_CLASS Devices[DEV_CLASS_MAX];
	WIN_TTY Terminals[WIN_TTY_MAX];
	WIN_PSTRING Strings[WIN_CHILD_MAX];
	WIN_MOUNT Mounts[WIN_MOUNT_MAX];
	WIN_GLOBALS Globals[1];
} WIN_SESSION;

/*
 * registry.c
 */

#define REG_DRIVER	1
#define REG_CLASS	2
#define REG_IFACE	3
#define REG_MOUNT	4
#define REG_TTY		5
