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

#include "arch_types.h"
#include "ole_types.h"

/* winerror.h */

#define ERROR_BAD_FILE_TYPE		222L		/* Invalid file system */
#define ERROR_ELEVATION_REQUIRED	740L		/* The requested operation requires elevation */
#define ERROR_SQL_ACCESS_DENIED		15105L		/* Vista panic: no access to error strings (de-elevating) */

/* winbase.h */

#define MAX_LABEL			32
#define MAX_NAME			64		/* ISO 9660 (Joliet) */
#define MAX_GUID			96
#define MAX_COMMENT			160
#define MAX_MESSAGE			256
#define MAX_TEXT			416
#define MAX_ARGBUF			0x07FFF		/* length of UNICODE_STRING struct */
//#define MAX_ARGBUF			0x40000		/* GNU configure */
#define MAX_FILESIZE			0xFFFFFFF0000	/* 16 TB minus 64 KB */

/* winnt.h */

/* We need at least FILE_WRITE_ATTRIBUTES to CreateHardLink() 
 * on a file with zero permissions (mutt_dotlock.exe).
 */

#define WIN_S_IREAD	(FILE_READ_DATA)
#define WIN_S_IWRITE	(FILE_WRITE_DATA | FILE_WRITE_EA | FILE_APPEND_DATA)
#define WIN_S_IEXEC	(FILE_EXECUTE)
#define WIN_S_IRX	(WIN_S_IREAD | WIN_S_IEXEC)
#define WIN_S_IRW	(WIN_S_IREAD | WIN_S_IWRITE)
#define WIN_S_IRWX	(WIN_S_IRW | WIN_S_IEXEC)

#define WIN_S_BLKSIZE	4096

#define SYSTEM_TIMEOFDAY_INFORMATION	SYSTEM_TIME_OF_DAY_INFORMATION

/* from Cygwin */

#define WIN_S_ISTXT	FILE_READ_DATA
#define WIN_S_ISGID	FILE_WRITE_DATA
#define WIN_S_ISUID	FILE_APPEND_DATA

/* wincon.h */

#define KeyEvent			Event.KeyEvent
#define MouseEvent			Event.MouseEvent
#define WindowBufferSizeEvent		Event.WindowBufferSizeEvent
#define MenuEvent			Event.MenuEvent
#define FocusEvent			Event.FocusEvent

#define ENABLE_VIRTUAL_TERMINAL_INPUT		0x0200

#define ENABLE_VIRTUAL_TERMINAL_PROCESSING	0x0004
#define ENABLE_LVB_GRID_WORLDWIDE		0x0010
#define DISABLE_NEWLINE_AUTO_RETURN		0x0008

#define WIN_MAX_INPUT		255

/* winternl.h */

typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER DpcTime;
    LARGE_INTEGER InterruptTime;
    ULONG InterruptCount;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

/*
 * win_acl.c
 */

#define ALL_INHERIT_ACE		(INHERITED_ACE | OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE)

#define WIN_ACL_TYPE_ACCESS		0x8000
#define WIN_ACL_TYPE_DEFAULT		0x4000

#define WIN_ACL_USER_OBJ		0x01
#define WIN_ACL_USER			0x02
#define WIN_ACL_GROUP_OBJ		0x04
#define WIN_ACL_GROUP			0x08
#define WIN_ACL_MASK			0x10	/* resolved Unix permissions? */
#define WIN_ACL_OTHER			0x20

/* Authority Identifiers */

#define SECURITY_NULL_SID_RID			0
#define SECURITY_WORLD_SID_RID			1
#define SECURITY_LOCAL_SID_RID			2
#define SECURITY_CREATOR_SID_RID		3
#define SECURITY_NT_AUTHORITY_RID		5
#define SECURITY_OTHER_AUTHORITY_RID		6

/* Vista */

#define SECURITY_PACKAGE_AUTHORITY_RID		15
#define SECURITY_MANDATORY_LABEL_AUTHORITY_RID	16

/* SubAuthority */

#define SECURITY_LOCAL_SERVICE_RID		19
#define SECURITY_LOCAL_NETWORK_RID		20
#define SECURITY_NT_SERVICE_RID			80

/* Domain */

#define DOMAIN_NT_SERVICE_RID_INSTALLER		0x876402C0	/* TrustedInstaller */

typedef struct _WIN_MODE {
	DWORD FileType;
	DWORD User;
	DWORD Group;
	DWORD Other;
	DWORD Special;
} WIN_MODE;

typedef struct _SID8 {
	BYTE Revision;
	BYTE SubAuthorityCount;
	SID_IDENTIFIER_AUTHORITY IdentifierAuthority;
	ULONG SubAuthority[8];
} SID8, *PSID8;

typedef struct _ACCESS_ALLOWED_ACE8 {
	ACE_HEADER Header;
  	ACCESS_MASK Mask;
	SID8 Sid;
} ACCESS_ALLOWED_ACE8;

typedef struct _ACE_PEEK {
	ACE_HEADER Header;
  	ACCESS_MASK Mask;
	SID Sid;
} ACE_PEEK;

typedef struct _WIN_ACL_CONTROL {
	SECURITY_DESCRIPTOR Security;
	SID8 Owner;
	SID8 Group;
	PACL Acl;
} WIN_ACL_CONTROL;

typedef struct _WIN_OBJECT_CONTROL {
	SECURITY_DESCRIPTOR Security;
	SID8 SidUser;
	ACL DefaultAcl;
	ACCESS_ALLOWED_ACE8 Ace[3];
} WIN_OBJECT_CONTROL;

/*
 * win_stdlib.c
 */

#define MIN_BUFSIZE		1024

#define WIN_MSGBUFSIZE		(4 * WIN_PAGE_SIZE)
#define WIN_ATEXIT_MAX		32

typedef VOID (*WIN_ATEXITPROC)(VOID);

/*
 * win_signal.c
 */

#define WM_STRING		0xC000			/* RegisterWindowMessage() */
#define WM_XPCONSOLE		(WM_STRING + 154)	/* 0xC09A */

#define WIN_NSIG		33

#define CTRL_C_EVENT				0
#define CTRL_BREAK_EVENT			1
#define CTRL_CLOSE_EVENT			2
#define CTRL_LOGOFF_EVENT			5
#define CTRL_SHUTDOWN_EVENT			6
#define CTRL_ACCESS_VIOLATION_EVENT		10
#define CTRL_ILLEGAL_INSTRUCTION_EVENT		11
#define CTRL_DIVIDE_BY_ZERO_EVENT		12
#define CTRL_SIZE_EVENT				13
#define CTRL_CHILD_EVENT			14
#define CTRL_TIMER_EVENT			15
#define CTRL_DETACH_EVENT			16
#define CTRL_QUIT_EVENT				17
#define CTRL_PIPE_EVENT				18
#define CTRL_ABORT_EVENT			19
#define CTRL_VTIMER_EVENT			20
#define CTRL_HANGUP_EVENT			21
#define CTRL_USER1_EVENT			22
#define CTRL_USER2_EVENT			23
#define CTRL_INVALID_ARGUMENT_EVENT		24
#define CTRL_MONITOR_EVENT			25
#define CTRL_EMULATOR_EVENT			26
#define CTRL_BUS_EVENT				27
#define CTRL_INFO_EVENT				28
#define CTRL_STOP_EVENT				29	/* Motor stop feature */
#define CTRL_CONTINUE_EVENT			30
#define CTRL_BACKGROUND_READ_EVENT		31
#define CTRL_BACKGROUND_WRITE_EVENT		32
#define CTRL_IO_EVENT				33
#define CTRL_URGENT_EVENT			34

typedef struct _WIN_SIGACTION {
	PVOID Function;
	UINT Mask;
	DWORD Flags;
} WIN_SIGACTION;

typedef BOOL (CALLBACK *WIN_SIGPROC)(DWORD, CONTEXT *);

/*
 * win_resource.c
 */

#define WIN_RLIMIT_CPU      0               /* cpu time in milliseconds */
#define WIN_RLIMIT_FSIZE    1               /* maximum file size */
#define WIN_RLIMIT_DATA     2               /* data size */
#define WIN_RLIMIT_STACK    3               /* stack size */
#define WIN_RLIMIT_CORE     4               /* core file size */
#define WIN_RLIMIT_RSS      5               /* resident set size */
#define WIN_RLIMIT_MEMLOCK  6               /* locked-in-memory address space */
#define WIN_RLIMIT_NPROC    7               /* number of processes */
#define WIN_RLIMIT_NOFILE   8               /* number of open files */
#define WIN_RLIM_NLIMITS    9               /* number of resource limits */

#define WIN_RLIM_INFINITY   (((DWORDLONG)1 << 63) - 1)

typedef struct _WIN_RLIMIT {
	DWORDLONG Current;
	DWORDLONG Maximum;
} WIN_RLIMIT;

typedef struct _WIN_RUSAGE {
	DWORDLONG Kernel;
	DWORDLONG User;
} WIN_RUSAGE;

/*
 * win_sched.c
 */

/* Init, svinit, sh, init.sh, systemd, upstart, openrc all can act as
 * init programs. (Rob Landley, OpenioTSummit, 2017, 1:24:30)
 */

#define WIN_PID_INIT	1
#define WIN_CHILD_MAX	80

#define WIN_P_IREAD	(GENERIC_READ)
#define WIN_P_IWRITE	(PROCESS_DUP_HANDLE)
#define WIN_P_IEXEC	(PROCESS_CREATE_THREAD | PROCESS_CREATE_PROCESS | PROCESS_TERMINATE | SYNCHRONIZE)
#define WIN_P_IRWX	(WIN_P_IREAD | WIN_P_IWRITE | WIN_P_IEXEC)

#define WIN_STACKSIZE	0x100000

#define WIN_SIDL	1	/* Process being created by fork. */
#define WIN_SRUN	2	/* Currently runnable. */
#define WIN_SSLEEP	3	/* Sleeping on an address. */
#define WIN_SSTOP	4	/* Process debugging or suspension. */
#define WIN_SZOMB	5	/* Awaiting collection by parent. */
#define WIN_SDEAD	6	/* Process is almost a zombie. */
#define WIN_SONPROC	7	/* Process is currently on a CPU. */

#define STARTF_PS_EXEC	0x00020000

typedef struct _WIN_THREAD_STRUCT {
	/* these are hardcoded (fork.S) */
	ULONG raddr;
	ULONG origin;
	ULONG size;
	ULONG source;
	ULONG offset;
	ULONG dest;
	/* these are dynamic */
	ULONG TaskId;
	HANDLE Token;
	HANDLE Handle;
	DWORD Flags;
	DWORD ThreadId;
	DWORD Result;
} WIN_THREAD_STRUCT;

typedef struct _WIN_USAGE {
	DWORD TaskId;
	DWORD Status;
} WIN_USAGE;

#define CURRENT	(ULONG)TlsGetValue(__TlsIndex)

/*
 * win_tls.c
 */

#define WIN_GOT_MAX		32

typedef struct _WIN_TLSENTRY {
	LPBYTE Address;
	DWORD Size;
} WIN_TLSENTRY;

static VOID NTAPI TlsMainCRTStartup(LPVOID hinstDLL, DWORD fdwReason, LPVOID lpvReserved);

/* 
 * win_pwd.c
 */

#define WIN_NETENUM_LOCAL		1
#define WIN_NETENUM_GLOBAL		2

typedef struct _WIN_PWENT {
	SID8 UserSid;
	SID8 GroupSid;
	DWORD Privileges;
	DWORD Integrity;
	DWORD Change;
	DWORD Expire;
	WCHAR Account[MAX_NAME];
	WCHAR Domain[MAX_NAME];
	CHAR Password[MAX_NAME];
	CHAR Comment[MAX_COMMENT];
	WCHAR Home[MAX_PATH];
} WIN_PWENT;

typedef struct _WIN_PWENUM {
	DWORD Count;
	DWORD Index;
	DWORD Total;
	DWORD Resume;
	PBYTE Data;
} WIN_PWENUM;

/*
 * win_grp.c
 */

typedef struct _WIN_GRENT {
	SID8 Sid;
	WCHAR Account[MAX_NAME];
	WCHAR Domain[MAX_NAME];
	CHAR Comment[MAX_COMMENT];
	CHAR Members[MAX_TEXT];
} WIN_GRENT;

/*
 * win_sysctl.c
 */

typedef struct _WIN_LOADAVG {
	DWORD Averages[3];
	LONG Scale;
} WIN_LOADAVG;

typedef struct _WIN_KINFO_PROC {
	FILETIME Created;
	FILETIME Exited;
	FILETIME Kernel;
	FILETIME User;
} WIN_KINFO_PROC;

/*
 * win_uio.c
 */

typedef struct _WIN_IOVEC {
	PVOID Buffer;
	SIZE_T Length;
} WIN_IOVEC;

/*
 * win_unistd.c
 */

typedef struct _WIN___TFORK {
} WIN___TFORK;
