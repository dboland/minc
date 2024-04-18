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
	"NDIS"
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
	}
	psz += msvc_sprintf(psz, "[0x%x])\n", Attribs);
	return(psz);
}
LPSTR 
VfsVolumeFlags(LPSTR Buffer, DWORD Flags, LPCSTR Label)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, "%s(0x%x): ", Label, Flags);
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
	psz = VfsFlagName(psz, FILE_VOLUME_IS_COMPRESSED, "VOLUME_IS_COMPRESSED", Flags, &Flags);
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
	psz += msvc_sprintf(psz, " remain(0x%x)\n", Flags);
	return(psz);
}
LPSTR 
VfsTermFlags(LPSTR Buffer, WIN_TERMIO *Mode, LPCSTR Label)
{
	LPSTR psz = Buffer;
	DWORD dwRemain;

	dwRemain = Mode->Input;
	psz += msvc_sprintf(psz, "%s(0): 0x%x: ", Label, dwRemain);
	psz = VfsFlagName(psz, WIN_INLCR, "WIN_INLCR", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, WIN_ICRNL, "WIN_ICRNL", dwRemain, &dwRemain);
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
	psz += msvc_sprintf(psz, " remain(0x%x)\n", dwRemain);

	dwRemain = Mode->Output;
	psz += msvc_sprintf(psz, "%s(1): 0x%x: ", Label, dwRemain);
	psz = VfsFlagName(psz, WIN_ONLCR, "WIN_ONLCR", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, WIN_OCRNL, "WIN_OCRNL", dwRemain, &dwRemain);
	psz = VfsFlagName(psz, WIN_OXTABS, "WIN_OXTABS", dwRemain, &dwRemain);
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
			strcpy(mask, "---");
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
			strcpy(mask, "---");
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
			strcpy(mask, "---");
			// 0x0200
			if (TestAccess(Perms, PROCESS_SET_INFORMATION, &dwRemain))
				mask[1] = 'w';
			// 0x0400
			if (TestAccess(Perms, PROCESS_QUERY_INFORMATION, &dwRemain))
				mask[0] = 'r';
			// 0x0800
			if (TestAccess(Perms, PROCESS_SUSPEND_RESUME, &dwRemain))
				mask[2] = 'x';
			psz += msvc_sprintf(psz, " process(%s) ", mask);
		}
		if (Type == OB_TYPE_FILE){
			/* 0x0004 */
			psz = VfsFlagName(psz, FILE_APPEND_DATA, "APPEND_DATA", dwRemain, &dwRemain);
			/* 0x0040 */
			psz = VfsFlagName(psz, FILE_DELETE_CHILD, "DELETE_CHILD", dwRemain, &dwRemain);
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
VfsPathFlags(LPSTR Buffer, DWORD Flags, LPCSTR Label)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, "%s(0x%x): ", Label, Flags);
	psz = VfsFlagName(psz, WIN_FOLLOW, "FOLLOW", Flags, &Flags);
	psz = VfsFlagName(psz, WIN_NOCROSSMOUNT, "NOCROSSMOUNT", Flags, &Flags);
	psz = VfsFlagName(psz, WIN_ISSYMLINK, "ISSYMLINK", Flags, &Flags);
	psz = VfsFlagName(psz, WIN_REQUIREDIR, "REQUIREDIR", Flags, &Flags);
	psz = VfsFlagName(psz, WIN_PATHCOPY, "PATHCOPY", Flags, &Flags);
	psz += msvc_sprintf(psz, " remain(0x%x)\n", Flags);
	return(psz);
}

/****************************************************/

VOID 
VfsDebugTask(WIN_TASK *Task, LPCSTR Label)
{
	msvc_printf("%s(%d): Command(%ls) Parent(%d) Flags(0x%x) Status(0x%x) Depth(%d) Process(%d) Thread(%d) Handle(%d) Session(%d) Group(%d) Terminal(%d)\n", 
		Label, Task->TaskId, __Strings[Task->TaskId].Command, Task->ParentId, Task->Flags, Task->Status, Task->Depth, Task->ProcessId, Task->ThreadId, Task->Handle, Task->SessionId, Task->GroupId, Task->TerminalId);
}
VOID 
VfsDebugStat(WIN_VATTR *Stat, LPCSTR Label)
{
	msvc_printf("%s(%d): Device(0x%x) Attribs(0x%x) Special(0x%x) Links(%d) Size(%d) Type(%s)\n", 
		Label, Stat->VolumeSerialNumber, Stat->DeviceId, Stat->Attributes, Stat->SpecialId, Stat->NumberOfLinks, Stat->FileSizeLow, __FType[Stat->Mode.FileType]);
}
VOID 
VfsDebugMount(WIN_MOUNT *Mount, LPCSTR Label)
{
	msvc_printf("%s(%d): Flags(0x%x) Serial(%lu) DevType(0x%x) DevId(0x%x)\n", 
		Label, Mount->MountId, Mount->Flags, Mount->VolumeSerial, Mount->DeviceType, Mount->DeviceId);
	msvc_printf("+ Path: %ls\n", Mount->Path);
	msvc_printf("+ Drive: %ls\n", Mount->Drive);
}
VOID 
VfsDebugPoll(WIN_VNODE *Node, WIN_POLLFD *Info, LPCSTR Label)
{
	msvc_printf("%s(%d): Type(%s:%s) Handle(%d) Device(0x%x) Events(0x%x) Result(0x%x) Access(0x%x)\n", 
		Label, Node->FileId, FSType(Node->FSType), __FType[Node->FileType], Node->Handle, Node->DeviceType, Info->Events, Info->Result, Node->Access);
}
VOID 
VfsDebugDrive(WIN_STATFS *Info, LPCSTR Label)
{
	msvc_printf("%s(%ls): Type(%ls) DevId(0x%x) Label(%ls) MaxPath(%d) BytesPerSector(%d)\n", 
		Label, Info->Drive, Info->TypeName, Info->DeviceId, Info->Label, Info->MaxPath, Info->BytesPerSector);
//	VfsVolumeFlags(Info->Flags, "  flags");
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
VfsDebugConfig(WIN_CFDATA *Config, LPCSTR Label)
{
	msvc_printf("%s(%ls): Bus(%ls) FSType(%s), DosPath(%ls) Depth(%d) Class(%ls) NtPath(%ls)\n", 
		Label, Config->NtName, Config->BusName, FSType(Config->FSType), Config->DosPath, Config->Depth, Config->ClassName, Config->NtPath);
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
//	msvc_printf("+ LocalBasePathOffsetUnicode(%d)\n", Info->LocalBasePathOffsetUnicode);
}
VOID 
VfsDebugThread(WIN_THREAD_STRUCT *Thread, LPSTR Label)
{
	msvc_printf("%s: return(0x%lx) origin(0x%lx) size(0x%lx) source(0x%lx) dest(0x%lx) offset(0x%lx) Token(%lu) Flags(0x%x)\n", 
		Label, Thread->raddr, Thread->origin, Thread->size, Thread->source, Thread->dest, Thread->offset, Thread->Token, Thread->Flags);
}

/****************************************************/

DWORD 
vfs_VNODE(WIN_VNODE *Node, LPSTR Buffer)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, "FileId(%d) Type(%s:%s) Handle(%d) Access(0x%x) CloEx(%d) DevType(0x%x) DevId(0x%x)\n", 
		Node->FileId, __FSType[Node->FSType], FType(Node->FileType), Node->Handle, Node->Access, Node->CloseExec, Node->DeviceType, Node->DeviceId);
	psz = VfsFileAttribs(psz, Node->Attribs);
	psz = VfsFileFlags(psz, Node->Flags);
	psz = VfsFileAccess(psz, Node->Access, OB_TYPE_FILE);
	return(psz - Buffer);
}
DWORD 
vfs_NAMEI(WIN_NAMEIDATA *Path, LPSTR Buffer)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, "Resolved(%ls): MountId(%d) Type(%s:%s)\n", 
		Path->Resolved, Path->MountId, FSType(Path->FSType), __FType[Path->FileType]);
	psz = VfsPathFlags(psz, Path->Flags, "+ Flags");
	psz = VfsFileAttribs(psz, Path->Attribs);
	return(psz - Buffer);
}
DWORD 
vfs_DEVICE(WIN_DEVICE *Device, LPSTR Buffer)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, "Name(%s) Type(%s:%s) Flags(0x%x) Handle(%d) Index(%d) I/O([%d][%d]) DevType(0x%x) DevId(0x%x)\n", 
		Device->Name, FSType(Device->FSType), __FType[Device->FileType], Device->Flags, Device->Handle, Device->Index, Device->Input, Device->Output, Device->DeviceType, Device->DeviceId);
	psz += msvc_sprintf(psz, "+ ClassId: %ls\n", Device->ClassId);
	psz += msvc_sprintf(psz, "+ NtName: %ls\n", Device->NtName);
	psz += msvc_sprintf(psz, "+ NtPath: %ls\n", Device->NtPath);
	return(psz - Buffer);
}
DWORD 
vfs_TERMIO(WIN_TTY *Terminal, LPSTR Buffer)
{
	LPSTR psz = Buffer;

	psz += msvc_sprintf(psz, "Id(%d) Device(0x%x) Group(%d) Session(%d) Row(%d) Col(%d)\n", 
		Terminal->TerminalId, Terminal->DeviceId, Terminal->GroupId, Terminal->SessionId, Terminal->WinSize.Row, Terminal->WinSize.Column);
	psz = VfsTermFlags(psz, &Terminal->Mode, "+ mode");
//	psz += vfs_ktrace_DEVICE(DEVICE(Terminal->DeviceId), "+ device", psz);
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
			vfs_TERMIO((WIN_TTY *)Data, szText);
			break;
	}
	msvc_printf("%s: %s", Label, szText);
}
