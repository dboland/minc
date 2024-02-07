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

#include <sys/msgbuf.h>

/****************************************************/

wchar_t *
cfexpand(wchar_t *string)
{
	wchar_t *result = string;
	wchar_t c;

	/* Expand Vista formatted strings by skipping them.
	 */
	while (c = *string++){
		if (c == ';' || c == '\\'){
			result = string;
		}
	}
	return(result);
}
int 
msgbuf_PDO(WIN_CFDATA *Config, WIN_CFDRIVER *Driver, LPSTR Result)
{
	LPSTR psz = Result;

	if (Driver->Flags & WIN_DVF_CONFIG_READY){
		psz += msvc_sprintf(psz, "%s on ", Driver->Name);
	}else{
		psz += msvc_sprintf(psz, "+ not configured: ");
	}
	psz += msvc_sprintf(psz, "%ls at %ls", Config->NtName, Config->BusName);
	psz += msvc_sprintf(psz, " %ls", cfexpand(Driver->Location));
	psz += msvc_sprintf(psz, ", class %ls", Driver->NtClass);
	psz += msvc_sprintf(psz, ", type 0x%x", Config->DeviceType);
	psz += msvc_sprintf(psz, ", flags 0x%x", Driver->Flags);
	psz += msvc_sprintf(psz, ", \"%ls\"", cfexpand(Driver->Comment));
	*psz++ = '\n';
	*psz = 0;
	return(psz - Result);
}
int 
msgbuf_DRIVE(WIN_CFDATA *Config, WIN_CFDRIVER *Driver, LPSTR Result)
{
	LPSTR psz = Result;

	if (Driver->Flags & WIN_DVF_CONFIG_READY){
		psz += msvc_sprintf(psz, "%s on ", Driver->Name);
	}else{
		psz += msvc_sprintf(psz, "+ not configured: ");
	}
	psz += msvc_sprintf(psz, "%ls at %ls", Config->NtName, Config->BusName);
	psz += msvc_sprintf(psz, " %ls", Driver->NtClass);
	psz += msvc_sprintf(psz, ", type 0x%x", Config->DeviceType);
	psz += msvc_sprintf(psz, ", flags 0x%x", Driver->Flags);
	*psz++ = '\n';
	*psz = 0;
	return(psz - Result);
}
int 
msgbuf_WINSOCK(WIN_IFDATA *Config, WIN_CFDRIVER *Driver, LPSTR Result)
{
	LPSTR psz = Result;

	if (Driver->Flags & WIN_DVF_CONFIG_READY){
		psz += msvc_sprintf(psz, "%s at ", Driver->Name);
	}else{
		psz += msvc_sprintf(psz, "+ not configured: ");
	}
	psz += msvc_sprintf(psz, "%ls", Config->NtName);
	psz += msvc_sprintf(psz, ", index %d", Config->Index);
	psz += msvc_sprintf(psz, ", type 0x%x", Config->DeviceType);
	psz += msvc_sprintf(psz, ", flags 0x%x", Driver->Flags);
	psz += msvc_sprintf(psz, ", \"%ls\"", Driver->Comment);
	*psz++ = '\n';
	*psz = 0;
	return(psz - Result);
}

/****************************************************/

int 
msgbuf_KERN_MSGBUFSIZE(int *data, size_t *len)
{
	WIN_CFDATA cfData;
	WIN_CFDRIVER cfDriver;
	DWORD dwFlags = WIN_MNT_VFSFLAGS;
	char *msgbuf = win_malloc(MSGBUFSIZE);
	size_t bufsize = 0;
	char *buf = msgbuf;

	if (!vfs_setvfs(&cfData, dwFlags)){
		return(-ECANCELED);
	}else while (vfs_getvfs(&cfData, dwFlags)){
		if (cfData.FSType == FS_TYPE_DRIVE){
			drive_statvfs(&cfData, dwFlags, &cfDriver);
			if (drive_match(cfData.NtName, cfData.DeviceType, &cfDriver)){
				bufsize += msgbuf_DRIVE(&cfData, &cfDriver, buf);
				msgbuf = win_realloc(msgbuf, bufsize + MSGBUFSIZE);
				buf = msgbuf + bufsize;
			}
		}else if (cfData.FSType == FS_TYPE_PDO){
			pdo_statvfs(&cfData, dwFlags, &cfDriver);
			if (pdo_match(cfData.NtName, cfData.DeviceType, &cfDriver)){
				bufsize += msgbuf_PDO(&cfData, &cfDriver, buf);
				msgbuf = win_realloc(msgbuf, bufsize + MSGBUFSIZE);
				buf = msgbuf + bufsize;
			}
		}
	}
	vfs_endvfs(&cfData);
	__Globals->MsgBufSize = bufsize;
	__Globals->MsgBuffer = msgbuf;
	*data = bufsize;
	return(0);
}
int 
msgbuf_KERN_MSGBUF(char data[], size_t *len)
{
	size_t bufsize = __Globals->MsgBufSize;
	char *msgbuf = __Globals->MsgBuffer;
	struct msgbuf *msg = (struct msgbuf *)data;

	msg->msg_magic = MSG_MAGIC;
	msg->msg_bufr = 0;
	msg->msg_bufl = bufsize;
	msg->msg_bufs = bufsize;
	msg->msg_bufx = bufsize;
	win_memcpy(&msg->msg_bufc, msgbuf, bufsize);
	win_free(msgbuf);
	return(0);
}
