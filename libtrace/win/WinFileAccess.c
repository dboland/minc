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

#include "config.h"

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
WinGenericFlags(LPSTR Buffer, ACCESS_MASK Access, DWORD Type, LPCSTR Label)
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
			psz += sprintf(psz, "%s(0x%08lx): file(%s)\n", Label, dwGeneric, mask);
		}else{
			psz += sprintf(psz, "%s(0x%08lx): process(%s)\n", Label, dwGeneric, mask);
		}
	}
	return(psz);
}
LPSTR 
WinReservedFlags(LPSTR Buffer, ACCESS_MASK Access, DWORD Type, LPCSTR Label)
{
	LPSTR psz = Buffer;
	ACCESS_MASK dwRemain = Access & 0x0F000000;

	if (dwRemain){
		psz += sprintf(psz, "%s(0x%08lx):", Label, dwRemain);
		psz = WinFlagName(psz, ACCESS_SYSTEM_SECURITY, "SYSTEM_SECURITY", dwRemain, &dwRemain);
		psz += sprintf(psz, "[0x%x]\n", dwRemain);
	}
	return(psz);
}
LPSTR 
WinStandardFlags(LPSTR Buffer, ACCESS_MASK Access, DWORD Type, LPCSTR Label)
{
	LPSTR psz = Buffer;
	ACCESS_MASK dwRemain = Access & 0x00FF0000;

	if (dwRemain){
		psz += sprintf(psz, "%s(0x%08lx): ", Label, dwRemain);
		psz = WinFlagName(psz, DELETE, "DELETE", dwRemain, &dwRemain);
		psz = WinFlagName(psz, READ_CONTROL, "READ_CONTROL", dwRemain, &dwRemain);
		psz = WinFlagName(psz, WRITE_DAC, "WRITE_DAC", dwRemain, &dwRemain);
		psz = WinFlagName(psz, WRITE_OWNER, "WRITE_OWNER", dwRemain, &dwRemain);
		psz = WinFlagName(psz, SYNCHRONIZE, "SYNCHRONIZE", dwRemain, &dwRemain);
		psz += sprintf(psz, "[0x%x]\n", dwRemain);
	}
	return(psz);
}
LPSTR 
WinSpecificFlags(LPSTR Buffer, ACCESS_MASK Perms, DWORD Type, LPCSTR Label)
{
	LPSTR psz = Buffer;
	char mask[4];
	ACCESS_MASK dwRemain = Perms & 0x0000FFFF;

	/* /c/MinGW/include/winnt.h */

	if (dwRemain){
		psz += sprintf(psz, "%s(0x%08lx): ", Label, dwRemain);
		strcpy(mask, "---");
		if (Type == OB_TYPE_FILE){
			/* 0x0001 */
			if (TestAccess(dwRemain, FILE_READ_DATA, &dwRemain))
				mask[0] = 'r';
			/* 0x0002 */
			if (TestAccess(dwRemain, FILE_WRITE_DATA, &dwRemain))
				mask[1] = 'w';
			/* 0x0020 */
			if (TestAccess(dwRemain, FILE_EXECUTE, &dwRemain))
				mask[2] = 'x';
			psz += sprintf(psz, "file(%s) ", mask);
		}else{
			/* 0x0001 */
			if (TestAccess(dwRemain, PROCESS_TERMINATE, &dwRemain))
				mask[0] = 'r';
			/* 0x0100 */
			if (TestAccess(dwRemain, PROCESS_SET_QUOTA, &dwRemain))
				mask[1] = 'w';
			/* 0x0800 */
			if (TestAccess(dwRemain, PROCESS_SUSPEND_RESUME, &dwRemain))
				mask[2] = 'x';
			psz += sprintf(psz, "process(%s) ", mask);
		}
		strcpy(mask, "---");
		if (Type == OB_TYPE_FILE){
			/* 0x0080 */
			if (TestAccess(dwRemain, FILE_READ_ATTRIBUTES, &dwRemain))
				mask[0] = 'r';
			/* 0x0100 */
			if (TestAccess(dwRemain, FILE_WRITE_ATTRIBUTES, &dwRemain))
				mask[1] = 'w';
		}else{
			/* 0x0200 */
			if (TestAccess(dwRemain, PROCESS_SET_INFORMATION, &dwRemain))
				mask[1] = 'w';
			/* 0x0400 */
			if (TestAccess(dwRemain, PROCESS_QUERY_INFORMATION, &dwRemain))
				mask[0] = 'r';
		}
		psz += sprintf(psz, "attr(%s) ", mask);
		strcpy(mask, "---");
		if (Type == OB_TYPE_FILE){
			/* 0x0008 */
			if (TestAccess(dwRemain, FILE_READ_EA, &dwRemain))
				mask[0] = 'r';
			/* 0x0010 */
			if (TestAccess(dwRemain, FILE_WRITE_EA, &dwRemain))
				mask[1] = 'w';
			psz += sprintf(psz, "xattr(%s) ", mask);
		}else{
			/* 0x0008 */
			if (TestAccess(dwRemain, PROCESS_VM_OPERATION, &dwRemain))
				mask[2] = 'x';
			/* 0x0010 */
			if (TestAccess(dwRemain, PROCESS_VM_READ, &dwRemain))
				mask[0] = 'r';
			/* 0x0020 */
			if (TestAccess(dwRemain, PROCESS_VM_WRITE, &dwRemain))
				mask[1] = 'w';
			psz += sprintf(psz, "vm(%s) ", mask);
		}
		if (Type == OB_TYPE_FILE){
			/* 0x0004 */
			psz = WinFlagName(psz, FILE_APPEND_DATA, "APPEND_DATA", dwRemain, &dwRemain);
			/* 0x0040 */
			psz = WinFlagName(psz, FILE_DELETE_CHILD, "DELETE_CHILD", dwRemain, &dwRemain);
			/* 0x0200 */
			psz = WinFlagName(psz, FILE_NO_EA_KNOWLEDGE, "NO_EA_KNOWLEDGE", dwRemain, &dwRemain);
		}else{
			/* 0x0004 */
			psz = WinFlagName(psz, PROCESS_SET_SESSIONID, "SET_SESSIONID", dwRemain, &dwRemain);
			/* 0x0002 */
			psz = WinFlagName(psz, PROCESS_CREATE_THREAD, "CREATE_THREAD", dwRemain, &dwRemain);
			/* 0x0040 */
			psz = WinFlagName(psz, PROCESS_DUP_HANDLE, "DUP_HANDLE", dwRemain, &dwRemain);
			/* 0x0080 */
			psz = WinFlagName(psz, PROCESS_CREATE_PROCESS, "CREATE_PROCESS", dwRemain, &dwRemain);
			/* 0x1000 */
			psz = WinFlagName(psz, PROCESS_QUERY_LIMITED_INFORMATION, "QUERY_LIMITED", dwRemain, &dwRemain);
		}
		psz += sprintf(psz, "[0x%x]\n", dwRemain);
	}
	return(psz);
}
LPSTR 
WinFileAccess(LPSTR Buffer, ACCESS_MASK Access, DWORD Type)
{
	LPSTR psz = Buffer;

	psz = WinGenericFlags(psz, Access, Type, "  generic");
	psz = WinReservedFlags(psz, Access, Type, "  reserved");
	psz = WinStandardFlags(psz, Access, Type, "  standard");
	psz = WinSpecificFlags(psz, Access, Type, "  specific");
	return(psz);
}
