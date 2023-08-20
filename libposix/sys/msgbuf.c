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

int 
msgbuf_KERN_MSGBUFSIZE(int *data, size_t *len)
{
	WIN_CFDATA cfData;
	WIN_CFDRIVER cfDriver;
	DWORD dwFlags = WIN_MNT_NOWAIT;
	char *msgbuf = win_malloc(MSGBUFSIZE);
	size_t bufsize = 0;
	char *buf = msgbuf;

	if (!vfs_setvfs(&cfData, dwFlags)){
		return(-1);
	}else while (vfs_getvfs(&cfData, dwFlags)){
		if (cfData.FSType == FS_TYPE_DEVICE){
			dev_statvfs(&cfData, dwFlags, &cfDriver);
			if (dev_match(cfData.NtName, cfDriver.DeviceType, &cfDriver)){
				bufsize += cfmessage(&cfData, &cfDriver, buf);
				msgbuf = win_realloc(msgbuf, bufsize + MSGBUFSIZE);
				buf = msgbuf + bufsize;
			}
		}
	}
	vfs_endvfs(&cfData);
	__Globals[WIN_KERN_MSGBUF].LowPart = bufsize;
	__Globals[WIN_KERN_MSGBUF].HighPart = (DWORD)msgbuf;
	*data = bufsize;
	return(0);
}
int 
msgbuf_KERN_MSGBUF(char data[], size_t *len)
{
	size_t bufsize = __Globals[WIN_KERN_MSGBUF].LowPart;
	char *msgbuf = (char *)__Globals[WIN_KERN_MSGBUF].HighPart;
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
