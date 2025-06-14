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

#include "../config.h"

/* Vista */

#define FILE_RETURNS_CLEANUP_RESULT_INFO	0x00000200
#define FILE_SUPPORTS_POSIX_UNLINK_RENAME	0x00000400

LPSTR 
VfsVolumeFlagsHigh(LPSTR Buffer, LPCSTR Label, DWORD Flags)
{
	LPSTR psz = Buffer;

	if (Flags){
		psz += sprintf(psz, "%s([0x%x]", Label, Flags);
		psz = WinFlagName(psz, WIN_MNT_DOOMED, "MNT_DOOMED", Flags, &Flags);
		psz = WinFlagName(psz, WIN_MNT_ROOTFS, "MNT_ROOTFS", Flags, &Flags);
		psz += sprintf(psz, "[0x%x])\n", Flags);
	}
	return(psz);
}
LPSTR 
VfsVolumeFlagsLow(LPSTR Buffer, LPCSTR Label, DWORD Flags)
{
	LPSTR psz = Buffer;

	psz += sprintf(psz, "%s([0x%x]", Label, Flags);
	psz = WinFlagName(psz, FILE_CASE_SENSITIVE_SEARCH, "CASE_SENSITIVE_SEARCH", Flags, &Flags);
	psz = WinFlagName(psz, FILE_CASE_PRESERVED_NAMES, "CASE_PRESERVED_NAMES", Flags, &Flags);
	psz = WinFlagName(psz, FILE_UNICODE_ON_DISK, "UNICODE_ON_DISK", Flags, &Flags);
	psz = WinFlagName(psz, FILE_PERSISTENT_ACLS, "PERSISTENT_ACLS", Flags, &Flags);
	psz = WinFlagName(psz, FILE_FILE_COMPRESSION, "FILE_COMPRESSION", Flags, &Flags);
	psz = WinFlagName(psz, FILE_VOLUME_QUOTAS, "VOLUME_QUOTAS", Flags, &Flags);
	psz = WinFlagName(psz, FILE_SUPPORTS_SPARSE_FILES, "SPARSE_FILES", Flags, &Flags);
	psz = WinFlagName(psz, FILE_SUPPORTS_REPARSE_POINTS, "REPARSE_POINTS", Flags, &Flags);
	psz = WinFlagName(psz, FILE_SUPPORTS_REMOTE_STORAGE, "REMOTE_STORAGE", Flags, &Flags);
	psz = WinFlagName(psz, FS_LFN_APIS, "FS_LFN_APIS", Flags, &Flags);
	psz = WinFlagName(psz, FILE_VOLUME_IS_COMPRESSED, "IS_COMPRESSED", Flags, &Flags);
	psz = WinFlagName(psz, FILE_SUPPORTS_OBJECT_IDS, "OBJECT_IDS", Flags, &Flags);
	psz = WinFlagName(psz, FILE_SUPPORTS_ENCRYPTION, "ENCRYPTION", Flags, &Flags);
	psz = WinFlagName(psz, FILE_NAMED_STREAMS, "NAMED_STREAMS", Flags, &Flags);
	psz = WinFlagName(psz, FILE_READ_ONLY_VOLUME, "READ_ONLY_VOLUME", Flags, &Flags);
	psz = WinFlagName(psz, FILE_SEQUENTIAL_WRITE_ONCE, "SEQUENTIAL_WRITE_ONCE", Flags, &Flags);
	psz = WinFlagName(psz, FILE_SUPPORTS_TRANSACTIONS, "TRANSACTIONS", Flags, &Flags);
	psz = WinFlagName(psz, FILE_SUPPORTS_HARD_LINKS, "HARD_LINKS", Flags, &Flags);
	psz = WinFlagName(psz, FILE_SUPPORTS_EXTENDED_ATTRIBUTES, "EXTENDED_ATTRIBUTES", Flags, &Flags);
	psz = WinFlagName(psz, FILE_SUPPORTS_OPEN_BY_FILE_ID, "OPEN_BY_FILE_ID", Flags, &Flags);
	psz = WinFlagName(psz, FILE_SUPPORTS_USN_JOURNAL, "USN_JOURNAL", Flags, &Flags);
	psz = WinFlagName(psz, FILE_SUPPORTS_POSIX_UNLINK_RENAME, "POSIX_UNLINK_RENAME", Flags, &Flags);
	psz = WinFlagName(psz, FILE_RETURNS_CLEANUP_RESULT_INFO, "CLEANUP_RESULT_INFO", Flags, &Flags);
	psz += sprintf(psz, "[0x%x])\n", Flags);
	return(psz);
}
