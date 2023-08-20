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

typedef struct _VOLUME_ID {
	DWORD VolumeIDSize;
	DWORD DriveType;
	DWORD DriveSerialNumber;
	DWORD VolumeLabelOffset;
} VOLUME_ID;

typedef struct _COMMON_NETWORK_RELATIVE_LINK {
	DWORD CommonNetworkRelativeLinkSize;
	DWORD CommonNetworkRelativeLinkFlags;
	DWORD NetNameOffset;
	DWORD DeviceNameOffset;
	DWORD NetworkProviderType;
} COMMON_NETWORK_RELATIVE_LINK;

typedef struct _LINK_INFO {
	DWORD LinkInfoSize;
	DWORD LinkInfoHeaderSize;
	DWORD LinkInfoFlags;
	DWORD VolumeIDOffset;
	DWORD LocalBasePathOffset;
	DWORD CommonNetworkRelativeLinkOffset;
	DWORD CommonPathSuffixOffset;
} LINK_INFO;

typedef struct _SHELL_LINK_HEADER {
	DWORD HeaderSize;
	CLSID LinkCLSID;
	DWORD LinkFlags;
	DWORD FileAttributes;
	FILETIME CreationTime;
	FILETIME AccessTime;
	FILETIME WriteTime;
	DWORD FileSize;
	DWORD IconIndex;
	DWORD ShowCommand;
	WORD HotKey;
	WORD Reserved1;
	DWORD Reserved2;
	DWORD Reserved3;
} SHELL_LINK_HEADER;

/* LinkFlags */

#define HasLinkTargetIDList		0x00000001
#define HasLinkInfo			0x00000002
#define HasName				0x00000004
#define IsUnicode			0x00000080

/* LinkInfoFlags */

#define VolumeIDAndLocalBasePath		0x1
#define CommonNetworkRelativeLinkAndPathSuffix	0x2

GUID CLSID_ShellLink = {0x00021401, 0, 0, {0xc0, 0, 0, 0, 0, 0, 0, 0x46}};

