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

#include <winbase.h>
#include <ddk/ntifs.h>	// Object types

#include "vfs_debug.h"

static const CHAR *__FSType[] = {
	"UNKNOWN", 
	"DISK", 
	"CHAR", 
	"PIPE", 
	"MAILSLOT", 
	"PDO", 
	"DRIVE", 
	"PROCESS", 
	"WINSOCK", 
	"REGISTRY", 
	"VOLUME", 
	"NPF",
	"LINK",
	"NDIS",
	"SHELL"
};
static const CHAR *__FType[] = {
	"VNON", 
	"VREG", 
	"VDIR", 
	"VBLK", 
	"VCHR", 
	"VLNK", 
	"VSOCK", 
	"VFIFO", 
	"VBAD"
};

/****************************************************/

LPCSTR 
FSType(WIN_FS_TYPE Type)
{
	LPCSTR pszResult;

	if (Type >= FS_TYPE_MAX){
		pszResult = "INVALID";
	}else{
		pszResult = __FSType[Type];
	}
	return(pszResult);
}
LPCSTR 
FType(WIN_VTYPE Type)
{
	LPCSTR pszResult;

	if (Type >= WIN_VMAX){
		pszResult = "INVALID";
	}else{
		pszResult = __FType[Type];
	}
	return(pszResult);
}
LPSTR 
VfsFlagName(LPSTR Buffer, DWORD Flag, LPCSTR Name, DWORD Mask, DWORD *Remain)
{
	LPSTR psz = Buffer;

	if (Mask & Flag){
		*Remain &= ~Flag;
		psz += msvc_sprintf(psz, "[%s]", Name);
	}
	return(psz);
}
LPSTR 
VfsFileFlags(LPSTR Buffer, DWORD Flags)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, "+ Flags([0x%x]", Flags);
	psz = VfsFlagName(psz, HANDLE_FLAG_INHERIT, "INHERIT", Flags, &Flags);
	psz = VfsFlagName(psz, HANDLE_FLAG_PROTECT_FROM_CLOSE, "PROTECT_FROM_CLOSE", Flags, &Flags);
	psz += msvc_sprintf(psz, "[0x%x])\n", Flags);
	return(psz);
}
LPSTR 
VfsFileAttribs(LPSTR Buffer, DWORD Attribs)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, "+ Attribs([0x%x]", Attribs);
	if (Attribs != -1){
		psz = VfsFlagName(psz, FILE_FLAG_BACKUP_SEMANTICS, "BACKUP_SEMANTICS", Attribs, &Attribs);
		psz = VfsFlagName(psz, FILE_FLAG_OVERLAPPED, "OVERLAPPED", Attribs, &Attribs);
		psz = VfsFlagName(psz, FILE_ATTRIBUTE_READONLY, "READONLY", Attribs, &Attribs);
		psz = VfsFlagName(psz, FILE_ATTRIBUTE_HIDDEN, "HIDDEN", Attribs, &Attribs);
		psz = VfsFlagName(psz, FILE_ATTRIBUTE_SYSTEM, "SYSTEM", Attribs, &Attribs);
		psz = VfsFlagName(psz, FILE_ATTRIBUTE_SYMLINK, "SYMLINK", Attribs, &Attribs);
		psz = VfsFlagName(psz, FILE_ATTRIBUTE_DIRECTORY, "DIRECTORY", Attribs, &Attribs);
		psz = VfsFlagName(psz, FILE_ATTRIBUTE_ARCHIVE, "ARCHIVE", Attribs, &Attribs);
		psz = VfsFlagName(psz, FILE_ATTRIBUTE_DEVICE, "DEVICE", Attribs, &Attribs);
		psz = VfsFlagName(psz, FILE_ATTRIBUTE_NORMAL, "NORMAL", Attribs, &Attribs);
		psz = VfsFlagName(psz, FILE_ATTRIBUTE_TEMPORARY, "TEMPORARY", Attribs, &Attribs);
		psz = VfsFlagName(psz, FILE_ATTRIBUTE_REPARSE_POINT, "REPARSE_POINT", Attribs, &Attribs);
		psz = VfsFlagName(psz, FILE_ATTRIBUTE_COMPRESSED, "COMPRESSED", Attribs, &Attribs);
		psz = VfsFlagName(psz, FILE_ATTRIBUTE_OFFLINE, "OFFLINE", Attribs, &Attribs);
		psz = VfsFlagName(psz, FILE_ATTRIBUTE_NOT_CONTENT_INDEXED, "NOT_CONTENT_INDEXED", Attribs, &Attribs);
		psz = VfsFlagName(psz, FILE_ATTRIBUTE_ENCRYPTED, "ENCRYPTED", Attribs, &Attribs);
		psz = VfsFlagName(psz, FILE_ATTRIBUTE_VIRTUAL, "VIRTUAL", Attribs, &Attribs);
	}
	psz += msvc_sprintf(psz, "[0x%x])\n", Attribs);
	return(psz);
}
LPSTR 
VfsVolumeFlags(LPSTR Buffer, LPCSTR Label, DWORD Flags)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, "%s([0x%x]", Label, Flags);
	psz = VfsFlagName(psz, FILE_CASE_SENSITIVE_SEARCH, "CASE_SENSITIVE_SEARCH", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_CASE_PRESERVED_NAMES, "CASE_PRESERVED_NAMES", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_UNICODE_ON_DISK, "UNICODE_ON_DISK", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_PERSISTENT_ACLS, "PERSISTENT_ACLS", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_FILE_COMPRESSION, "FILE_COMPRESSION", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_VOLUME_QUOTAS, "VOLUME_QUOTAS", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_SUPPORTS_SPARSE_FILES, "SPARSE_FILES", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_SUPPORTS_REPARSE_POINTS, "REPARSE_POINTS", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_SUPPORTS_REMOTE_STORAGE, "REMOTE_STORAGE", Flags, &Flags);
	psz = VfsFlagName(psz, FS_LFN_APIS, "FS_LFN_APIS", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_VOLUME_IS_COMPRESSED, "IS_COMPRESSED", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_SUPPORTS_OBJECT_IDS, "OBJECT_IDS", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_SUPPORTS_ENCRYPTION, "ENCRYPTION", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_NAMED_STREAMS, "NAMED_STREAMS", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_READ_ONLY_VOLUME, "READ_ONLY_VOLUME", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_SEQUENTIAL_WRITE_ONCE, "SEQUENTIAL_WRITE_ONCE", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_SUPPORTS_TRANSACTIONS, "TRANSACTIONS", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_SUPPORTS_HARD_LINKS, "HARD_LINKS", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_SUPPORTS_EXTENDED_ATTRIBUTES, "EXTENDED_ATTRIBUTES", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_SUPPORTS_OPEN_BY_FILE_ID, "OPEN_BY_FILE_ID", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_SUPPORTS_USN_JOURNAL, "USN_JOURNAL", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_VOLUME_MNT_DOOMED, "VOLUME_MNT_DOOMED", Flags, &Flags);
	psz = VfsFlagName(psz, FILE_VOLUME_MNT_ROOTFS, "VOLUME_MNT_ROOTFS", Flags, &Flags);
	psz += msvc_sprintf(psz, "[0x%x])\n", Flags);
	return(psz);
}
LPSTR 
VfsInputMode(LPSTR Buffer, LPCSTR Label, DWORD Mode)
{
	LPSTR psz = Buffer;
	DWORD dwRemain = Mode;

	psz += msvc_sprintf(psz, "%s(input): [0x%x]", Label, dwRemain);
	psz = VfsFlagName(psz, ENABLE_ECHO_INPUT, "ECHO_INPUT", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, ENABLE_INSERT_MODE, "INSERT_MODE", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, ENABLE_LINE_INPUT, "LINE_INPUT", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, ENABLE_MOUSE_INPUT, "MOUSE_INPUT", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, ENABLE_PROCESSED_INPUT, "PROCESSED_INPUT", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, ENABLE_QUICK_EDIT_MODE, "QUICK_EDIT_MODE", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, ENABLE_WINDOW_INPUT, "WINDOW_INPUT", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, ENABLE_VIRTUAL_TERMINAL_INPUT, "VIRTUAL_TERMINAL_INPUT", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, ENABLE_EXTENDED_FLAGS, "EXTENDED_FLAGS", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, ENABLE_AUTO_POSITION, "AUTO_POSITION", dwRemain, &dwRemain);
	psz += msvc_sprintf(psz, "[0x%x]\n", dwRemain);
	return(psz);
}
LPSTR 
VfsScreenMode(LPSTR Buffer, LPCSTR Label, DWORD Mode)
{
	LPSTR psz = Buffer;
	DWORD dwRemain = Mode;

	psz += msvc_sprintf(psz, "%s(screen): 0x%x: ", Label, dwRemain);
	psz = VfsFlagName(psz, ENABLE_PROCESSED_OUTPUT, "PROCESSED_OUTPUT", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, ENABLE_WRAP_AT_EOL_OUTPUT, "WRAP_AT_EOL_OUTPUT", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, ENABLE_VIRTUAL_TERMINAL_PROCESSING, "VIRTUAL_TERMINAL_PROCESSING", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, DISABLE_NEWLINE_AUTO_RETURN, "DISABLE_NEWLINE_AUTO_RETURN", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, ENABLE_LVB_GRID_WORLDWIDE, "LVB_GRID_WORLDWIDE", dwRemain, &dwRemain);
	psz += msvc_sprintf(psz, " remain(0x%x)\n", dwRemain);
	return(psz);
}
LPSTR 
VfsNetFlags(LPSTR Buffer, LONG NetworkEvents, LPCSTR Label)
{
	LPSTR psz = Buffer;
	DWORD dwRemain = NetworkEvents;

	psz += msvc_sprintf(psz, "%s(0x%x): ", Label, NetworkEvents);
	psz = VfsFlagName(psz, FD_READ, "READ", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, FD_WRITE, "WRITE", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, FD_OOB, "OOB", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, FD_ACCEPT, "ACCEPT", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, FD_CONNECT, "CONNECT", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, FD_CLOSE, "CLOSE", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, FD_QOS_BIT, "QOS_BIT", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, FD_GROUP_QOS, "GROUP_QOS", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, FD_ROUTING_INTERFACE_CHANGE, "ROUTING_INTERFACE_CHANGE", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, FD_ADDRESS_LIST_CHANGE, "ADDRESS_LIST_CHANGE", dwRemain, &dwRemain);
	psz += msvc_sprintf(psz, " remain(0x%x)\n", dwRemain);
	return(psz);
}
BOOL 
TestAccess(ACCESS_MASK Mask, ACCESS_MASK Access, ACCESS_MASK *Remain)
{
	BOOL bResult = FALSE;

	if (Mask & Access){
		*Remain &= ~Access;
		bResult = TRUE;
	}
	return(bResult);
}
LPSTR 
VfsGenericFlags(LPSTR Buffer, ACCESS_MASK Access, DWORD Type, LPCSTR Label)
{
	LPSTR psz = Buffer;
	char mask[4] = "---";
	ACCESS_MASK dwGeneric = Access & 0xF0000000;

	if (dwGeneric){
		if (dwGeneric & GENERIC_ALL){				// 0x10000000
			strcpy(mask, "rwx");
		}else{
			if (Access & GENERIC_READ) mask[0] = 'r';	// 0x80000000
			if (Access & GENERIC_WRITE) mask[1] = 'w';	// 0x40000000
			if (Access & GENERIC_EXECUTE) mask[2] = 'x';	// 0x20000000
		}
		if (Type == OB_TYPE_FILE){
			psz += msvc_sprintf(psz, "%s(0x%08lx): file(%s)\n", Label, dwGeneric, mask);
		}else{
			psz += msvc_sprintf(psz, "%s(0x%08lx): process(%s)\n", Label, dwGeneric, mask);
		}
	}
	return(psz);
}
LPSTR 
VfsReservedFlags(LPSTR Buffer, ACCESS_MASK Access, DWORD Type, LPCSTR Label)
{
	LPSTR psz = Buffer;
	ACCESS_MASK dwRemain = Access & 0x0F000000;

	if (dwRemain){
		psz += msvc_sprintf(psz, "%s(0x%08lx):", Label, dwRemain);
		psz = VfsFlagName(psz, ACCESS_SYSTEM_SECURITY, "SYSTEM_SECURITY", dwRemain, &dwRemain);
		psz += msvc_sprintf(psz, " remain(0x%x)\n", dwRemain);
	}
	return(psz);
}
LPSTR 
VfsStandardFlags(LPSTR Buffer, ACCESS_MASK Access, DWORD Type, LPCSTR Label)
{
	LPSTR psz = Buffer;
	ACCESS_MASK dwRemain = Access & 0x00FF0000;

	if (dwRemain){
		psz += msvc_sprintf(psz, "%s(0x%08lx): ", Label, dwRemain);
		psz = VfsFlagName(psz, DELETE, "DELETE", dwRemain, &dwRemain);
		psz = VfsFlagName(psz, READ_CONTROL, "READ_CONTROL", dwRemain, &dwRemain);
		psz = VfsFlagName(psz, WRITE_DAC, "WRITE_DAC", dwRemain, &dwRemain);
		psz = VfsFlagName(psz, WRITE_OWNER, "WRITE_OWNER", dwRemain, &dwRemain);
		psz = VfsFlagName(psz, SYNCHRONIZE, "SYNCHRONIZE", dwRemain, &dwRemain);
		psz += msvc_sprintf(psz, " remain(0x%x)\n", dwRemain);
	}
	return(psz);
}
LPSTR 
VfsSpecificFlags(LPSTR Buffer, ACCESS_MASK Perms, DWORD Type, LPCSTR Label)
{
	LPSTR psz = Buffer;
	char mask[4];
	ACCESS_MASK dwRemain = Perms & 0x0000FFFF;

	/* /c/MinGW/include/winnt.h */

	if (dwRemain){
		psz += msvc_sprintf(psz, "%s(0x%08lx): ", Label, dwRemain);
		if (Type == OB_TYPE_FILE){
			win_strcpy(mask, "---");
			// 0x0001
			if (TestAccess(Perms, FILE_READ_DATA, &dwRemain))
				mask[0] = 'r';
			// 0x0002
			if (TestAccess(Perms, FILE_WRITE_DATA, &dwRemain))
				mask[1] = 'w';
			// 0x0020
			if (TestAccess(Perms, FILE_EXECUTE, &dwRemain))
				mask[2] = 'x';
			psz += msvc_sprintf(psz, "file(%s) ", mask);
		}else{
			psz = VfsFlagName(psz, PROCESS_TERMINATE, "TERMINATE", dwRemain, &dwRemain);
			psz = VfsFlagName(psz, PROCESS_CREATE_THREAD, "CREATE_THREAD", dwRemain, &dwRemain);
		}
		if (Type == OB_TYPE_FILE){
			win_strcpy(mask, "---");
			// 0x0080
			if (TestAccess(Perms, FILE_READ_ATTRIBUTES, &dwRemain))
				mask[0] = 'r';
			// 0x0100
			if (TestAccess(Perms, FILE_WRITE_ATTRIBUTES, &dwRemain))
				mask[1] = 'w';
			psz += msvc_sprintf(psz, "attr(%s) ", mask);
		}else{
			psz = VfsFlagName(psz, PROCESS_CREATE_PROCESS, "CREATE_PROCESS", dwRemain, &dwRemain);
			psz = VfsFlagName(psz, PROCESS_SET_QUOTA, "SET_QUOTA", dwRemain, &dwRemain);
		}
		strcpy(mask, "---");
		if (Type == OB_TYPE_FILE){
			// 0x0008
			if (TestAccess(Perms, FILE_READ_EA, &dwRemain))
				mask[0] = 'r';
			// 0x0010
			if (TestAccess(Perms, FILE_WRITE_EA, &dwRemain))
				mask[1] = 'w';
			psz += msvc_sprintf(psz, "xattr(%s) ", mask);
		}else{
			// 0x0008
			if (TestAccess(Perms, PROCESS_VM_OPERATION, &dwRemain))
				mask[2] = 'x';
			// 0x0010
			if (TestAccess(Perms, PROCESS_VM_READ, &dwRemain))
				mask[0] = 'r';
			// 0x0020
			if (TestAccess(Perms, PROCESS_VM_WRITE, &dwRemain))
				mask[1] = 'w';
			psz += msvc_sprintf(psz, " vm(%s)", mask);
		}
		if (Type != OB_TYPE_FILE){
			win_strcpy(mask, "---");
			/* 0x0200 */
			if (TestAccess(Perms, PROCESS_SET_INFORMATION, &dwRemain))
				mask[1] = 'w';
			/* 0x0400 */
			if (TestAccess(Perms, PROCESS_QUERY_INFORMATION, &dwRemain))
				mask[0] = 'r';
			/* 0x0800 */
			if (TestAccess(Perms, PROCESS_SUSPEND_RESUME, &dwRemain))
				mask[2] = 'x';
			psz += msvc_sprintf(psz, " process(%s) ", mask);
		}
		if (Type == OB_TYPE_FILE){
			/* 0x0004 */
			psz = VfsFlagName(psz, FILE_APPEND_DATA, "APPEND_DATA", dwRemain, &dwRemain);
			/* 0x0040 */
			psz = VfsFlagName(psz, FILE_DELETE_CHILD, "DELETE_CHILD", dwRemain, &dwRemain);
			/* 0x0200 */
			psz = VfsFlagName(psz, FILE_NO_EA_KNOWLEDGE, "NO_EA_KNOWLEDGE", dwRemain, &dwRemain);
		}else{
			/* 0x0004 */
			psz = VfsFlagName(psz, TOKEN_IMPERSONATE, "IMPERSONATE", dwRemain, &dwRemain);
			/* 0x0040 */
			psz = VfsFlagName(psz, PROCESS_DUP_HANDLE, "DUP_HANDLE", dwRemain, &dwRemain);
		}
		psz = VfsFlagName(psz, PROCESS_QUERY_LIMITED_INFORMATION, "QUERY_LIMITED", dwRemain, &dwRemain);
		psz += msvc_sprintf(psz, " remain(0x%x)\n", dwRemain);
	}
	return(psz);
}
LPSTR 
VfsFileAccess(LPSTR Buffer, ACCESS_MASK Access, DWORD Type)
{
	LPSTR psz = Buffer;

	psz = VfsGenericFlags(psz, Access, Type, "+ generic");
	psz = VfsReservedFlags(psz, Access, Type, "+ reserved");
	psz = VfsStandardFlags(psz, Access, Type, "+ standard");
	psz = VfsSpecificFlags(psz, Access, Type, "+ specific");
	return(psz);
}
LPSTR 
VfsPathFlags(LPSTR Buffer, LPCSTR Label, DWORD Flags)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, "%s([0x%x]", Label, Flags);
	psz = VfsFlagName(psz, WIN_FOLLOW, "FOLLOW", Flags, &Flags);
	psz = VfsFlagName(psz, WIN_NOCROSSMOUNT, "NOCROSSMOUNT", Flags, &Flags);
	psz = VfsFlagName(psz, WIN_ISSYMLINK, "ISSYMLINK", Flags, &Flags);
	psz = VfsFlagName(psz, WIN_REQUIREDIR, "REQUIREDIR", Flags, &Flags);
	psz = VfsFlagName(psz, WIN_PATHCOPY, "PATHCOPY", Flags, &Flags);
	psz = VfsFlagName(psz, WIN_STRIPSLASHES, "STRIPSLASHES", Flags, &Flags);
	psz += msvc_sprintf(psz, "[0x%x])\n", Flags);
	return(psz);
}

/****************************************************/

VOID 
VfsDebugTask(WIN_TASK *Task, LPCSTR Label)
{
	msvc_printf("%s(%d): Command(%ls) Parent(%d) Flags(0x%x) Status(0x%x) Depth(%d) Process(%d) Thread(%d) Handle(%d) Session(%d) Group(%d) CTTY(%d)\n", 
		Label, Task->TaskId, __Strings[Task->TaskId].Command, Task->ParentId, Task->Flags, Task->Status, Task->Depth, Task->ProcessId, Task->ThreadId, Task->Handle, Task->SessionId, Task->GroupId, Task->CTTY);
}
VOID 
VfsDebugStat(WIN_VATTR *Stat, LPCSTR Label)
{
	msvc_printf("%s(%d): Device(0x%x) Attribs(0x%x) Special(0x%x) Links(%d) Size(%d) Type(%s)\n", 
		Label, Stat->VolumeSerialNumber, Stat->DeviceId, Stat->Attributes, Stat->SpecialId, Stat->NumberOfLinks, Stat->FileSizeLow, __FType[Stat->Mode.FileType]);
}
VOID 
VfsDebugPoll(WIN_VNODE *Node, WIN_POLLFD *Info, LPCSTR Label)
{
	msvc_printf("%s(%d): Type(%s:%s) Handle(%d) Device(0x%x) Events(0x%x) Result(0x%x) Access(0x%x)\n", 
		Label, Node->FileId, FSType(Node->FSType), __FType[Node->FileType], Node->Handle, Node->DeviceType, Info->Events, Info->Result, Node->Access);
}
VOID 
VfsDebugInterface(MIB_IFROW *Interface, LPCSTR Label)
{
	msvc_printf("%s(%ls): Descript(%s)\n", 
		Label, Interface->wszName, Interface->bDescr);
}
VOID 
VfsDebugTime(SYSTEMTIME *Time, LPCSTR Label)
{
	msvc_printf("%s: wYear(%d) wMonth(%d) wDayOfWeek(%d) wDay(%d) wHour(%d) wMinute(%d) wSecond(%d) wMilliseconds(%d)\n",
		Label, Time->wYear, Time->wMonth, Time->wDayOfWeek, Time->wDay, Time->wHour, Time->wMinute, Time->wSecond, Time->wMilliseconds);
}
VOID 
VfsDebugTimer(WIN_TASK *Task, LPCSTR Label)
{
	msvc_printf("%s(%d): Timer(%d) Interval(%d) Ticks(%I64d)\n", 
		Label, Task->TaskId, Task->Timer, Task->Interval, Task->Ticks);
}
VOID 
VfsDebugLink(LINK_INFO *Info, LPSTR Label)
{
	msvc_printf("%s(%d): LinkInfoHeaderSize(%d) Flags(0x%x) VolumeIDOffset(%d)\n", 
		Label, sizeof(SHELL_LINK_HEADER), Info->LinkInfoHeaderSize, Info->LinkInfoFlags, Info->VolumeIDOffset);
	msvc_printf("+ LocalBasePathOffset(%d)\n", Info->LocalBasePathOffset);
	msvc_printf("+ CommonNetworkRelativeLinkOffset(%d)\n", Info->CommonNetworkRelativeLinkOffset);
	msvc_printf("+ CommonPathSuffixOffset(%d)\n", Info->CommonPathSuffixOffset);
}
VOID 
VfsDebugThread(WIN_THREAD_STRUCT *Thread, LPSTR Label)
{
	msvc_printf("%s: return(0x%lx) origin(0x%lx) size(0x%lx) source(0x%lx) dest(0x%lx) offset(0x%lx) Token(%lu) Flags(0x%x)\n", 
		Label, Thread->raddr, Thread->origin, Thread->size, Thread->source, Thread->dest, Thread->offset, Thread->Token, Thread->Flags);
}
VOID 
VfsDebugLinkFlags(DWORD Flags)
{
	msvc_printf("+ Flags([0x%x]", Flags);
	win_flagname(HasLinkTargetIDList, "HasLinkTargetIDList", Flags, &Flags);
	win_flagname(HasLinkInfo, "HasLinkInfo", Flags, &Flags);
	win_flagname(HasName, "HasName", Flags, &Flags);
	win_flagname(HasRelativePath, "HasRelativePath", Flags, &Flags);
	win_flagname(HasWorkingDir, "HasWorkingDir", Flags, &Flags);
	win_flagname(HasArguments, "HasArguments", Flags, &Flags);
	win_flagname(HasIconLocation, "HasIconLocation", Flags, &Flags);
	win_flagname(IsUnicode, "IsUnicode", Flags, &Flags);
	win_flagname(ForceNoLinkInfo, "ForceNoLinkInfo", Flags, &Flags);
	msvc_printf("[0x%x])\n", Flags);
}

/****************************************************/

DWORD 
vfs_VNODE(WIN_VNODE *Node, LPSTR Buffer)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, "Type(%s:%s) Handle(%d) Object(%d) Event(%d) Index(%d) Access(0x%x) CloEx(%d)\n", 
		FSType(Node->FSType), FType(Node->FileType), Node->Handle, Node->Object, Node->Event, Node->Index, Node->Access, Node->CloseExec);
	psz += msvc_sprintf(psz, "+ Device: Type(0x%x) Id(0x%x)\n", Node->DeviceType, Node->DeviceId);
	psz = VfsFileAttribs(psz, Node->Attribs);
	psz = VfsFileFlags(psz, Node->Flags);
	psz = VfsFileAccess(psz, Node->Access, OB_TYPE_FILE);
	return(psz - Buffer);
}
DWORD 
vfs_NAMEI(WIN_NAMEIDATA *Path, LPSTR Buffer)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, "Resolved(%ls): MountId(%d) Object(%d) Size(%d) Type(%s:%s) DeviceId(0x%x)\n", 
		Path->Resolved, Path->MountId, Path->Object, Path->Size, FSType(Path->FSType), FType(Path->FileType), Path->DeviceId);
	psz = VfsPathFlags(psz, "+ Flags", Path->Flags);
	psz = VfsFileAttribs(psz, Path->Attribs);
	psz += msvc_sprintf(psz, "+ Base: %ls\n", Path->Base);
	return(psz - Buffer);
}
DWORD 
vfs_DEVICE(WIN_DEVICE *Device, LPSTR Buffer)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, "Name(%s) Type(%s:%s) Flags(0x%x) Index(%d) I/O([%d][%d]) DevType(0x%x) DevId(0x%x)\n", 
		Device->Name, FSType(Device->FSType), __FType[Device->FileType], Device->Flags, Device->Index, Device->Input, Device->Output, Device->DeviceType, Device->DeviceId);
	psz += msvc_sprintf(psz, "+ ClassId: %ls\n", Device->ClassId);
	psz += msvc_sprintf(psz, "+ NtName: %ls\n", Device->NtName);
	psz += msvc_sprintf(psz, "+ NtPath: %ls\n", Device->NtPath);
	return(psz - Buffer);
}
DWORD 
vfs_TTY(WIN_TTY *Terminal, LPSTR Buffer)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, "Index(%d) Name(%s) Device(0x%x) Flags(0x%x) Group(%d) Session(%d) Row(%d) Col(%d)\n", 
		Terminal->Index, Terminal->Name, Terminal->DeviceId, Terminal->Flags, Terminal->GroupId, Terminal->SessionId, Terminal->WinSize.Row, Terminal->WinSize.Column);
//	psz = VfsTermFlags(psz, &Terminal->Mode, "+ mode");
	return(psz - Buffer);
}
DWORD 
vfs_CONIN(DWORD Mode, LPSTR Buffer)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, "\n");
	psz = VfsInputMode(psz, "+ mode", Mode);
	return(psz - Buffer);
}
DWORD 
vfs_MOUNT(WIN_MOUNT *Mount, LPSTR Buffer)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, "TypeName(%ls) MountId(%d) MaxPath(%d) DevType(0x%x) DevId(0x%x)\n", 
		Mount->TypeName, Mount->MountId, Mount->MaxPath, Mount->DeviceType, Mount->DeviceId);
	psz = VfsVolumeFlags(psz, "+ Flags", Mount->Flags);
	psz += msvc_sprintf(psz, "+ Serial: %lu\n", Mount->Serial);
	psz += msvc_sprintf(psz, "+ Label: %ls\n", Mount->Label);
	psz += msvc_sprintf(psz, "+ Volume: %ls\n", Mount->Volume);
	return(psz - Buffer);
}
DWORD 
vfs_CFDATA(WIN_CFDATA *Config, LPSTR Buffer)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, "NtName(%ls): Bus(%ls) FSType(%s), DosPath(%ls) Depth(%d) Class(%ls)\n", 
		Config->NtName, Config->BusName, FSType(Config->FSType), Config->DosPath, Config->Depth, Config->ClassName);
	psz += msvc_sprintf(psz, "+ NtPath: %ls\n", Config->NtPath);
	return(psz - Buffer);
}
DWORD 
vfs_STATFS(WIN_STATFS *Info, LPSTR Buffer)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, "Type(%ls) DevId(0x%x) MaxPath(%d) ClustersTotal(%d) FreeClusters(%d)\n", 
		Info->TypeName, Info->DeviceId, Info->MaxPath, Info->ClustersTotal, Info->FreeClusters);
	psz = VfsVolumeFlags(psz, "+ Flags", Info->Flags);
	psz += msvc_sprintf(psz, "+ Path: %ls\n", Info->Path);
	return(psz - Buffer);
}

/****************************************************/

VOID 
vfs_ktrace(LPCSTR Label, STRUCT_TYPE Type, PVOID Data)
{
	CHAR szText[MAX_TEXT];

	switch (Type){
		case STRUCT_VNODE:
			vfs_VNODE((WIN_VNODE *)Data, szText);
			break;
		case STRUCT_NAMEI:
			vfs_NAMEI((WIN_NAMEIDATA *)Data, szText);
			break;
		case STRUCT_DEVICE:
			vfs_DEVICE((WIN_DEVICE *)Data, szText);
			break;
		case STRUCT_TTY:
			vfs_TTY((WIN_TTY *)Data, szText);
			break;
		case STRUCT_MOUNT:
			vfs_MOUNT((WIN_MOUNT *)Data, szText);
			break;
		case DWORD_CONIN:
			vfs_CONIN(*(DWORD *)Data, szText);
			break;
		case STRUCT_CFDATA:
			vfs_CFDATA((WIN_CFDATA *)Data, szText);
			break;
		case STRUCT_STATFS:
			vfs_STATFS((WIN_STATFS *)Data, szText);
			break;
	}
	msvc_printf("%s: %s", Label, szText);
}
