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

/****************************************************/

DWORD 
TTYCarriageReturn(LPSTR Buffer, UINT Flags)
{
	DWORD dwCount = 0;

	if (Flags & WIN_OCRNL){
		Buffer[dwCount++] = '\n';
	}else{
		Buffer[dwCount++] = '\r';
	}
	return(dwCount);
}
DWORD 
TTYLineFeed(LPSTR Buffer, UINT Flags)
{
	DWORD dwCount = 0;

	if (Flags & WIN_ONLCR){
		Buffer[dwCount++] = '\r';
	}
	Buffer[dwCount++] = '\n';
	return(dwCount);
}

/****************************************************/

BOOL 
tty_open(WIN_TTY *Terminal, WIN_FLAGS *Flags, WIN_VNODE *Result)
{
//	Result->FSType = FS_TYPE_TERMINAL;
	Result->DeviceType = Terminal->DeviceType;
	Result->DeviceId = Terminal->DeviceId;
	Result->Index = Terminal->Index;
//vfs_ktrace("tty_open", STRUCT_VNODE, Result);
	return(TRUE);
}
BOOL 
tty_write(WIN_TTY *Terminal, LPCSTR Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = TRUE;
	LONG lResult = 0;
	DWORD dwCount;
	UINT uiFlags = Terminal->Mode.OFlags;
	WIN_DEVICE *pwDevice = DEVICE(Terminal->DeviceId);
	CHAR szBuffer[4];
	UCHAR C;

	while (lResult < Size){
		C = *Buffer++;
		dwCount = 1;
		switch (C){
			case '\n':
				dwCount = TTYLineFeed(szBuffer, uiFlags);
				break;
			case '\r':
				dwCount = TTYCarriageReturn(szBuffer, uiFlags);
				break;
			default:
				*szBuffer = C;
		}
		if (!pdo_write(pwDevice, szBuffer, dwCount, &dwCount)){
			break;
		}
		lResult++;
	}
	*Result = lResult;
	return(bResult);
}
BOOL 
tty_read(WIN_TTY *Terminal, LPSTR Buffer, DWORD Size, DWORD *Result)
{
	BOOL bResult = FALSE;
	LONG lSize = Size;
	DWORD dwResult = 0;
	DWORD dwCount;
	WIN_DEVICE *pwDevice = DEVICE(Terminal->DeviceId);
	DWORD dwOffset = Terminal->Offset;
	CHAR *pszBuffer = Terminal->Buffer + dwOffset;
	UCHAR C;

	while (!bResult){
		if (lSize < 1){
			bResult = TRUE;
		}else if (C = *pszBuffer){
			*Buffer++ = C;
			pszBuffer++;
			dwResult++;
			lSize--;
			dwOffset++;
		}else if (pdo_read(pwDevice, Terminal->Buffer, WIN_MAX_INPUT, &dwCount)){
			pszBuffer = Terminal->Buffer;
			pszBuffer[dwCount] = 0;
			dwOffset = 0;
		}else{
			break;
		}
	}
	Terminal->Offset = dwOffset;
	*Result = dwResult;
	return(bResult);
}
