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

static const CHAR *_FSType[] = {
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
static const CHAR *_FType[] = {
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
		pszResult = _FSType[Type];
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
		pszResult = _FType[Type];
	}
	return(pszResult);
}
VOID 
VfsFileAttribs(DWORD Attribs, LPCWSTR Label)
{
	msvc_printf("%ls(0x%x): ", Label, Attribs);
	if (Attribs != -1){
		win_flagname(FILE_FLAG_BACKUP_SEMANTICS, "BACKUP_SEMANTICS", Attribs, &Attribs);
		win_flagname(FILE_FLAG_OVERLAPPED, "OVERLAPPED", Attribs, &Attribs);
		win_flagname(FILE_ATTRIBUTE_READONLY, "READONLY", Attribs, &Attribs);
		win_flagname(FILE_ATTRIBUTE_HIDDEN, "HIDDEN", Attribs, &Attribs);
		win_flagname(FILE_ATTRIBUTE_SYSTEM, "SYSTEM", Attribs, &Attribs);
		win_flagname(FILE_ATTRIBUTE_SYMLINK, "SYMLINK", Attribs, &Attribs);
		win_flagname(FILE_ATTRIBUTE_DIRECTORY, "DIRECTORY", Attribs, &Attribs);
		win_flagname(FILE_ATTRIBUTE_ARCHIVE, "ARCHIVE", Attribs, &Attribs);
		win_flagname(FILE_ATTRIBUTE_DEVICE, "DEVICE", Attribs, &Attribs);
		win_flagname(FILE_ATTRIBUTE_NORMAL, "NORMAL", Attribs, &Attribs);
		win_flagname(FILE_ATTRIBUTE_TEMPORARY, "TEMPORARY", Attribs, &Attribs);
		win_flagname(FILE_ATTRIBUTE_REPARSE_POINT, "REPARSE_POINT", Attribs, &Attribs);
		win_flagname(FILE_ATTRIBUTE_COMPRESSED, "COMPRESSED", Attribs, &Attribs);
		win_flagname(FILE_ATTRIBUTE_OFFLINE, "OFFLINE", Attribs, &Attribs);
	}
	msvc_printf(" remain(0x%x)\n", Attribs);
}
VOID 
VfsFileFlags(DWORD Flags, LPCWSTR Label)
{
	msvc_printf("%ls(0x%x): ", Label, Flags);
	win_flagname(HANDLE_FLAG_INHERIT, "INHERIT", Flags, &Flags);
	win_flagname(HANDLE_FLAG_PROTECT_FROM_CLOSE, "PROTECT_FROM_CLOSE", Flags, &Flags);
	msvc_printf(" remain(0x%x)\n", Flags);
}
VOID 
VfsVolumeFlags(DWORD Flags, LPCSTR Label)
{
	msvc_printf("%s(0x%x): ", Label, Flags);
	win_flagname(FILE_CASE_SENSITIVE_SEARCH, "CASE_SENSITIVE_SEARCH", Flags, &Flags);
	win_flagname(FILE_CASE_PRESERVED_NAMES, "CASE_PRESERVED_NAMES", Flags, &Flags);
	win_flagname(FILE_UNICODE_ON_DISK, "UNICODE_ON_DISK", Flags, &Flags);
	win_flagname(FILE_PERSISTENT_ACLS, "PERSISTENT_ACLS", Flags, &Flags);
	win_flagname(FILE_FILE_COMPRESSION, "FILE_COMPRESSION", Flags, &Flags);
	win_flagname(FILE_VOLUME_QUOTAS, "VOLUME_QUOTAS", Flags, &Flags);
	win_flagname(FILE_SUPPORTS_SPARSE_FILES, "SPARSE_FILES", Flags, &Flags);
	win_flagname(FILE_SUPPORTS_REPARSE_POINTS, "REPARSE_POINTS", Flags, &Flags);
	win_flagname(FILE_SUPPORTS_REMOTE_STORAGE, "REMOTE_STORAGE", Flags, &Flags);
	win_flagname(FS_LFN_APIS, "FS_LFN_APIS", Flags, &Flags);
	win_flagname(FILE_VOLUME_IS_COMPRESSED, "VOLUME_IS_COMPRESSED", Flags, &Flags);
	win_flagname(FILE_SUPPORTS_OBJECT_IDS, "OBJECT_IDS", Flags, &Flags);
	win_flagname(FILE_SUPPORTS_ENCRYPTION, "ENCRYPTION", Flags, &Flags);
	win_flagname(FILE_NAMED_STREAMS, "NAMED_STREAMS", Flags, &Flags);
	win_flagname(FILE_READ_ONLY_VOLUME, "READ_ONLY_VOLUME", Flags, &Flags);
	win_flagname(FILE_SEQUENTIAL_WRITE_ONCE, "SEQUENTIAL_WRITE_ONCE", Flags, &Flags);
	win_flagname(FILE_SUPPORTS_TRANSACTIONS, "TRANSACTIONS", Flags, &Flags);
	win_flagname(FILE_SUPPORTS_HARD_LINKS, "HARD_LINKS", Flags, &Flags);
	win_flagname(FILE_SUPPORTS_EXTENDED_ATTRIBUTES, "EXTENDED_ATTRIBUTES", Flags, &Flags);
	win_flagname(FILE_SUPPORTS_OPEN_BY_FILE_ID, "OPEN_BY_FILE_ID", Flags, &Flags);
	win_flagname(FILE_SUPPORTS_USN_JOURNAL, "USN_JOURNAL", Flags, &Flags);
	msvc_printf(" remain(0x%x)\n", Flags);
}
VOID 
VfsTermFlags(DWORD Mode[2], LPCSTR Label)
{
	DWORD dwRemain;

	dwRemain = Mode[0];
	msvc_printf("%s(0): 0x%x: ", Label, dwRemain);
	win_flagname(WIN_INLCR, "WIN_INLCR", dwRemain, &dwRemain);
	win_flagname(WIN_ICRNL, "WIN_ICRNL", dwRemain, &dwRemain);
	win_flagname(ENABLE_ECHO_INPUT, "ECHO_INPUT", dwRemain, &dwRemain);
	win_flagname(ENABLE_INSERT_MODE, "INSERT_MODE", dwRemain, &dwRemain);
	win_flagname(ENABLE_LINE_INPUT, "LINE_INPUT", dwRemain, &dwRemain);
	win_flagname(ENABLE_MOUSE_INPUT, "MOUSE_INPUT", dwRemain, &dwRemain);
	win_flagname(ENABLE_PROCESSED_INPUT, "PROCESSED_INPUT", dwRemain, &dwRemain);
	win_flagname(ENABLE_QUICK_EDIT_MODE, "QUICK_EDIT_MODE", dwRemain, &dwRemain);
	win_flagname(ENABLE_WINDOW_INPUT, "WINDOW_INPUT", dwRemain, &dwRemain);
	win_flagname(ENABLE_VIRTUAL_TERMINAL_INPUT, "VIRTUAL_TERMINAL_INPUT", dwRemain, &dwRemain);
	win_flagname(ENABLE_EXTENDED_FLAGS, "EXTENDED_FLAGS", dwRemain, &dwRemain);
	win_flagname(ENABLE_AUTO_POSITION, "AUTO_POSITION", dwRemain, &dwRemain);
	msvc_printf(" remain(0x%x)\n", dwRemain);

	dwRemain = Mode[1];
	msvc_printf("%s(1): 0x%x: ", Label, dwRemain);
	win_flagname(WIN_ONLCR, "WIN_ONLCR", dwRemain, &dwRemain);
	win_flagname(WIN_OCRNL, "WIN_OCRNL", dwRemain, &dwRemain);
	win_flagname(WIN_OXTABS, "WIN_OXTABS", dwRemain, &dwRemain);
	win_flagname(ENABLE_PROCESSED_OUTPUT, "PROCESSED_OUTPUT", dwRemain, &dwRemain);
	win_flagname(ENABLE_WRAP_AT_EOL_OUTPUT, "WRAP_AT_EOL_OUTPUT", dwRemain, &dwRemain);
	win_flagname(ENABLE_VIRTUAL_TERMINAL_PROCESSING, "VIRTUAL_TERMINAL_PROCESSING", dwRemain, &dwRemain);
	win_flagname(DISABLE_NEWLINE_AUTO_RETURN, "DISABLE_NEWLINE_AUTO_RETURN", dwRemain, &dwRemain);
	win_flagname(ENABLE_LVB_GRID_WORLDWIDE, "LVB_GRID_WORLDWIDE", dwRemain, &dwRemain);
	msvc_printf(" remain(0x%x)\n", dwRemain);
}
VOID 
VfsNetFlags(LONG NetworkEvents, LPCSTR Label)
{
	DWORD dwRemain = NetworkEvents;

	msvc_printf("%s(0x%x): ", Label, NetworkEvents);
	win_flagname(FD_READ, "READ", dwRemain, &dwRemain);
	win_flagname(FD_WRITE, "WRITE", dwRemain, &dwRemain);
	win_flagname(FD_OOB, "OOB", dwRemain, &dwRemain);
	win_flagname(FD_ACCEPT, "ACCEPT", dwRemain, &dwRemain);
	win_flagname(FD_CONNECT, "CONNECT", dwRemain, &dwRemain);
	win_flagname(FD_CLOSE, "CLOSE", dwRemain, &dwRemain);
	win_flagname(FD_QOS_BIT, "QOS_BIT", dwRemain, &dwRemain);
	win_flagname(FD_GROUP_QOS, "GROUP_QOS", dwRemain, &dwRemain);
	win_flagname(FD_ROUTING_INTERFACE_CHANGE, "ROUTING_INTERFACE_CHANGE", dwRemain, &dwRemain);
	win_flagname(FD_ADDRESS_LIST_CHANGE, "ADDRESS_LIST_CHANGE", dwRemain, &dwRemain);
	msvc_printf(" remain(0x%x)\n", dwRemain);
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
VOID 
VfsGenericFlags(ACCESS_MASK Access, DWORD Type, LPCSTR Label)
{
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
			msvc_printf("%s(0x%08lx): file(%s)\n", Label, dwGeneric, mask);
		}else{
			msvc_printf("%s(0x%08lx): process(%s)\n", Label, dwGeneric, mask);
		}
	}
}
VOID 
VfsReservedFlags(ACCESS_MASK Access, DWORD Type, LPCSTR Label)
{
	ACCESS_MASK dwRemain = Access & 0x0F000000;

	if (dwRemain){
		msvc_printf("%s(0x%08lx):", Label, dwRemain);
		win_flagname(ACCESS_SYSTEM_SECURITY, "SYSTEM_SECURITY", dwRemain, &dwRemain);
		msvc_printf(" remain(0x%x)\n", dwRemain);
	}
}
VOID 
VfsStandardFlags(ACCESS_MASK Access, DWORD Type, LPCSTR Label)
{
	ACCESS_MASK dwRemain = Access & 0x00FF0000;

	if (dwRemain){
		msvc_printf("%s(0x%08lx): ", Label, dwRemain);
		win_flagname(DELETE, "DELETE", dwRemain, &dwRemain);
		win_flagname(READ_CONTROL, "READ_CONTROL", dwRemain, &dwRemain);
		win_flagname(WRITE_DAC, "WRITE_DAC", dwRemain, &dwRemain);
		win_flagname(WRITE_OWNER, "WRITE_OWNER", dwRemain, &dwRemain);
		win_flagname(SYNCHRONIZE, "SYNCHRONIZE", dwRemain, &dwRemain);
		msvc_printf(" remain(0x%x)\n", dwRemain);
	}
}
VOID 
VfsSpecificFlags(ACCESS_MASK Perms, DWORD Type, LPCSTR Label)
{
	/* /c/MinGW/include/winnt.h */

	char mask[4];
	ACCESS_MASK dwRemain = Perms & 0x0000FFFF;

	if (dwRemain){
		msvc_printf("%s(0x%08lx): ", Label, dwRemain);
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
			msvc_printf("file(%s) ", mask);
		}else{
			win_flagname(PROCESS_TERMINATE, "TERMINATE", dwRemain, &dwRemain);
			win_flagname(PROCESS_CREATE_THREAD, "CREATE_THREAD", dwRemain, &dwRemain);
		}
		if (Type == OB_TYPE_FILE){
			strcpy(mask, "---");
			// 0x0080
			if (TestAccess(Perms, FILE_READ_ATTRIBUTES, &dwRemain))
				mask[0] = 'r';
			// 0x0100
			if (TestAccess(Perms, FILE_WRITE_ATTRIBUTES, &dwRemain))
				mask[1] = 'w';
			msvc_printf("attr(%s) ", mask);
		}else{
			win_flagname(PROCESS_CREATE_PROCESS, "CREATE_PROCESS", dwRemain, &dwRemain);
			win_flagname(PROCESS_SET_QUOTA, "SET_QUOTA", dwRemain, &dwRemain);
		}
		strcpy(mask, "---");
		if (Type == OB_TYPE_FILE){
			// 0x0008
			if (TestAccess(Perms, FILE_READ_EA, &dwRemain))
				mask[0] = 'r';
			// 0x0010
			if (TestAccess(Perms, FILE_WRITE_EA, &dwRemain))
				mask[1] = 'w';
			msvc_printf("xattr(%s) ", mask);
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
			msvc_printf(" vm(%s)", mask);
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
			msvc_printf(" process(%s) ", mask);
		}
		if (Type == OB_TYPE_FILE){
			/* 0x0004 */
			win_flagname(FILE_APPEND_DATA, "APPEND_DATA", dwRemain, &dwRemain);
			/* 0x0040 */
			win_flagname(FILE_DELETE_CHILD, "DELETE_CHILD", dwRemain, &dwRemain);
		}else{
			/* 0x0004 */
			win_flagname(TOKEN_IMPERSONATE, "IMPERSONATE", dwRemain, &dwRemain);
			/* 0x0040 */
			win_flagname(PROCESS_DUP_HANDLE, "DUP_HANDLE", dwRemain, &dwRemain);
		}
		win_flagname(PROCESS_QUERY_LIMITED_INFORMATION, "QUERY_LIMITED", dwRemain, &dwRemain);
		msvc_printf(" remain(0x%x)\n", dwRemain);
	}
}
VOID 
VfsAccessFlags(ACCESS_MASK Access, DWORD Type)
{
	VfsGenericFlags(Access, Type, "+ generic");
	VfsReservedFlags(Access, Type, "+ reserved");
	VfsStandardFlags(Access, Type, "+ standard");
	VfsSpecificFlags(Access, Type, "+ specific");
}
VOID 
VfsPathFlags(DWORD Flags, LPCSTR Label)
{
	msvc_printf("%s(0x%x): ", Label, Flags);
	win_flagname(WIN_FOLLOW, "FOLLOW", Flags, &Flags);
	win_flagname(WIN_NOCROSSMOUNT, "NOCROSSMOUNT", Flags, &Flags);
	win_flagname(WIN_ISSYMLINK, "ISSYMLINK", Flags, &Flags);
	win_flagname(WIN_REQUIREDIR, "REQUIREDIR", Flags, &Flags);
//	win_flagname(WIN_REQUIREDRIVE, "REQUIREDRIVE", Flags, &Flags);
	win_flagname(WIN_PATHCOPY, "PATHCOPY", Flags, &Flags);
	msvc_printf(" remain(0x%x)\n", Flags);
}

/****************************************************/

VOID 
VfsDebugPath(WIN_NAMEIDATA *Path, LPCSTR Label)
{
	msvc_printf("%s(%ls): MountId(%d) Type(%s:%s)\n", 
		Label, Path->Resolved, Path->MountId, FSType(Path->FSType), _FType[Path->FileType]);
	VfsPathFlags(Path->Flags, "+ Flags");
	VfsFileAttribs(Path->Attribs, L"+ Attribs");
	msvc_printf("+ Base: %ls\n", Path->Base);
	msvc_printf("+ S: %ls\n", Path->S);
}
VOID 
VfsDebugNode(WIN_VNODE *Node, LPCSTR Label)
{
	msvc_printf("%s(%d): Type(%s:%s) Handle(%d) Event(%d) Access(0x%x) CloEx(%d) DevType(0x%x) DevId(0x%x)\n", 
		Label, Node->FileId, FSType(Node->FSType), FType(Node->FileType), Node->Handle, Node->Event, Node->Access, Node->CloseExec, Node->DeviceType, Node->DeviceId);
	VfsFileFlags(Node->Flags, L"+ Flags");
	VfsFileAttribs(Node->Attribs, L"+ Attribs");
	VfsAccessFlags(Node->Access, OB_TYPE_FILE);
}
VOID 
VfsDebugDevice(WIN_DEVICE *Device, LPCSTR Label)
{
	msvc_printf("%s(%s): Flags(0x%x) Handle(%d) Index(%d) Type(%s:%s) I/O([%d][%d]) DevType(0x%x) DevId(0x%x)\n", 
		Label, Device->Name, Device->Flags, Device->Handle, Device->Index, FSType(Device->FSType), _FType[Device->FileType], Device->Input, Device->Output, Device->DeviceType, Device->DeviceId);
	msvc_printf("+ ClassId: %ls\n", Device->ClassId);
	msvc_printf("+ NtName: %ls\n", Device->NtName);
	msvc_printf("+ NtPath: %ls\n", Device->NtPath);
}
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
		Label, Stat->VolumeSerialNumber, Stat->DeviceId, Stat->Attributes, Stat->SpecialId, Stat->NumberOfLinks, Stat->FileSizeLow, _FType[Stat->Mode.FileType]);
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
		Label, Node->FileId, FSType(Node->FSType), _FType[Node->FileType], Node->Handle, Node->DeviceType, Info->Events, Info->Result, Node->Access);
}
VOID 
VfsDebugTTY(WIN_TERMIO *Terminal, LPCSTR Label)
{
	msvc_printf("%s(%d): Device(0x%x) Group(%d) Session(%d) Row(%d) Col(%d)\n", 
		Label, Terminal->TerminalId, Terminal->DeviceId, Terminal->GroupId, Terminal->SessionId, Terminal->WinSize.Row, Terminal->WinSize.Column);
	VfsTermFlags(Terminal->Mode, "+ mode");
	VfsDebugDevice(DEVICE(Terminal->DeviceId), "+ device");
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
