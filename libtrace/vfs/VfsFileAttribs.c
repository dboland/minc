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

LPSTR 
VfsFileAttribs(LPSTR Buffer, DWORD Attribs)
{
	LPSTR psz = Buffer;

	psz += sprintf(psz, "  Attribs([0x%x]", Attribs);
	if (Attribs != -1){
		psz = WinFlagName(psz, FILE_FLAG_BACKUP_SEMANTICS, "BACKUP_SEMANTICS", Attribs, &Attribs);
		psz = WinFlagName(psz, FILE_FLAG_OVERLAPPED, "OVERLAPPED", Attribs, &Attribs);
		psz = WinFlagName(psz, FILE_ATTRIBUTE_READONLY, "READONLY", Attribs, &Attribs);
		psz = WinFlagName(psz, FILE_ATTRIBUTE_HIDDEN, "HIDDEN", Attribs, &Attribs);
		psz = WinFlagName(psz, FILE_ATTRIBUTE_SYSTEM, "SYSTEM", Attribs, &Attribs);
		psz = WinFlagName(psz, FILE_ATTRIBUTE_LABEL, "LABEL", Attribs, &Attribs);
		psz = WinFlagName(psz, FILE_ATTRIBUTE_DIRECTORY, "DIRECTORY", Attribs, &Attribs);
		psz = WinFlagName(psz, FILE_ATTRIBUTE_ARCHIVE, "ARCHIVE", Attribs, &Attribs);
		psz = WinFlagName(psz, FILE_ATTRIBUTE_DEVICE, "DEVICE", Attribs, &Attribs);
		psz = WinFlagName(psz, FILE_ATTRIBUTE_NORMAL, "NORMAL", Attribs, &Attribs);
		psz = WinFlagName(psz, FILE_ATTRIBUTE_TEMPORARY, "TEMPORARY", Attribs, &Attribs);
		psz = WinFlagName(psz, FILE_ATTRIBUTE_REPARSE_POINT, "REPARSE_POINT", Attribs, &Attribs);
		psz = WinFlagName(psz, FILE_ATTRIBUTE_COMPRESSED, "COMPRESSED", Attribs, &Attribs);
		psz = WinFlagName(psz, FILE_ATTRIBUTE_OFFLINE, "OFFLINE", Attribs, &Attribs);
		psz = WinFlagName(psz, FILE_ATTRIBUTE_NOT_CONTENT_INDEXED, "NOT_CONTENT_INDEXED", Attribs, &Attribs);
		psz = WinFlagName(psz, FILE_ATTRIBUTE_ENCRYPTED, "ENCRYPTED", Attribs, &Attribs);
		psz = WinFlagName(psz, FILE_ATTRIBUTE_VIRTUAL, "VIRTUAL", Attribs, &Attribs);
	}
	psz += sprintf(psz, "[0x%x])\n", Attribs);
	return(psz);
}
