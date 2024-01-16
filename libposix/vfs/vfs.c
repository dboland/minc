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

/* iphlpapi.h */
#define _WIN32_WINNT	_WIN32_WINNT_WINXP

/* prevent __declspec(dllimport) decoration */
#define NTOSAPI		

#include <winsock2.h>
#include <ddk/ntapi.h>
#include <iphlpapi.h>	/* Win32 network api */

#include <stdint.h>

#include "win_posix.h"
#include "msvc_posix.h"
#include "ws2_types.h"
#include "ole_types.h"
#include "dev_types.h"
#include "vfs_types.h"

#define WIN_ERR			msvc_printf
#define OBJECT_NAME(name)	"Local\\MinC_" VERSION "_" name

extern DWORD 	__TlsIndex;

extern SID8 SidSystem;
extern SID8 SidAdmins;
extern SID8 SidNone;
extern SID8 SidEveryone;
extern SID8 SidAuthenticated;
extern SID8 SidMachine;
extern SID8 SidUsers;
extern SID8 SidGuests;
extern SID8 SidBuiltin;

typedef struct _SEQUENCE {
	CHAR *Args;
	LONG Arg1;
	CHAR Char1;
	CHAR CSI;
	CHAR Buf[64];
} SEQUENCE;

HANDLE 		__Shared;
WIN_SIGPROC	__SignalProc;
HANDLE		__Interrupt;
HANDLE		__PipeEvent;
HANDLE		__MailEvent;
DWORD		__TaskId;
SEQUENCE 	__ANSI_BUF;
CHAR		__INPUT_BUF[WIN_MAX_INPUT + 2];
UCHAR 		__Char;
HGLOBAL		__Lock;			/* Clipboard lock */

WIN_SESSION	*__Session;
WIN_TASK	*__Tasks;
WIN_DEV_CLASS	*__Devices;
WIN_TERMIO	*__Terminals;
WIN_MOUNT	*__Mounts;
WIN_PSTRING	*__Strings;
LARGE_INTEGER	*__Globals;
WIN_TERMIO 	*__CTTY;
CHAR 		*__Escape;
CHAR		*__Input = __INPUT_BUF;
CONST CHAR	*__Clipboard;		/* Clipboard buffer */

BOOL vfs_namei(HANDLE Handle, DWORD Index, WIN_VNODE *Result);
BOOL proc_poll(VOID);

#include "vfs_acl.c"
#include "vfs_debug.c"
#include "vfs_signal.c"
#include "vfs_statvfs.c"
#include "vfs_libgen.c"
#include "vfs_device.c"
#include "registry/registry.c"
#include "pipe/pipe.c"
#include "char/char.c"
#include "drive/drive.c"
#include "disk/disk.c"
#include "pdo/pdo.c"
#include "volume/volume.c"
#include "mailslot/mailslot.c"
#include "event/event.c"
#include "winsock/winsock.c"
#include "vfs_fcntl.c"
#include "vfs_stdlib.c"
#include "vfs_unistd.c"
#include "vfs_syscall.c"
#include "vfs_namei.c"
#include "process/process.c"
#include "vfs_uio.c"
#include "vfs_filio.c"
#include "vfs_stat.c"
#include "vfs_dirent.c"
#include "vfs_poll.c"
#include "vfs_termio.c"
#include "vfs_socket.c"
#include "vfs_shm.c"
#include "vfs_time.c"
#include "vfs_wait.c"
#include "vfs_ktrace.c"
#include "vfs_resource.c"
//#include "vfs_ldt.c"

/****************************************************/

BOOL 
vfs_PROCESS_ATTACH(HINSTANCE Instance, LPVOID Reserved)
{
	__TlsIndex = TlsAlloc();
	__Session = vfs_shm_init(OBJECT_NAME("Session"), Instance);
	__Tasks = __Session->Tasks;
	__Devices = __Session->Devices;
	__Terminals = __Session->Terminals;
	__Mounts = __Session->Mounts;
	__Strings = __Session->Strings;
	__Globals = __Session->Globals;
	__PipeEvent = event_attach(OBJECT_NAME("PipeEvent"), FALSE);
	__MailEvent = event_attach(OBJECT_NAME("MailEvent"), FALSE);
	__Interrupt = event_attach(OBJECT_NAME("Interrupt"), FALSE);
	win_acl_PROCESS_ATTACH();
//	win_ldt_attach(WIN_CHILD_MAX + 1);
	return(TRUE);
}
BOOL 
vfs_PROCESS_DETACH(WIN_TASK *Task)
{
	vfs_shm_finish(__Session);
	TlsFree(__TlsIndex);
	return(TRUE);
}

/****************************************************/

BOOL 
vfs_init(WIN_FS_TYPE FileSystem)
{
	BOOL bResult = FALSE;

	switch (FileSystem){
		case FS_TYPE_UNKNOWN:
			bResult = TRUE;
			break;
		case FS_TYPE_WINSOCK:
			bResult = ws2_init();
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
BOOL 
vfs_finish(WIN_FS_TYPE FileSystem)
{
	BOOL bResult = FALSE;

	switch (FileSystem){
		case FS_TYPE_UNKNOWN:
			bResult = TRUE;
			break;
		case FS_TYPE_WINSOCK:
			bResult = ws2_finish();
			break;
		default:
			SetLastError(ERROR_BAD_FILE_TYPE);
	}
	return(bResult);
}
