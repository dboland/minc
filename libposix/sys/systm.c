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

#include <sys/systm.h>

/* Devices are loaded using OpenBSD's autoconf framework. After executing the 
 * biosboot program, devices are known, but need to be matched with their drivers.
 * The matching process is started by calling cpu_configure() which
 * calls config_rootfound() once, which searches the tree by
 * calling config_search(), which calls the xxx_match() function of all the
 * drivers in its parent class.
 * After that, config_search() calls the xxx_attach() function of the found driver,
 * which calls config_found(), completing driver initialization.
 */

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
cfmessage(WIN_CFDATA *Config, WIN_CFDRIVER *Driver, LPSTR Result)
{
	LPSTR psz = Result;

	if (Driver->Flags){
		psz += msvc_sprintf(psz, "%s on ", Driver->Name);
	}else{
		psz += msvc_sprintf(psz, "+ not configured: ");
	}
	psz += msvc_sprintf(psz, "%ls at %ls", Config->NtName, Config->BusName);
	psz += msvc_sprintf(psz, " %ls", cfexpand(Driver->Location));
	psz += msvc_sprintf(psz, ", type 0x%x", Driver->DeviceType);
	psz += msvc_sprintf(psz, ", driver %ls:%ls", Driver->NtClass, Driver->Service);
	psz += msvc_sprintf(psz, ", \"%ls\"", cfexpand(Driver->Comment));
	*psz++ = '\n';
	*psz = 0;
	return(psz - Result);
}

/****************************************************/

void 
cpu_configure(void)
{
	WIN_CFDATA cfData;
	WIN_CFDRIVER cfDriver;
	DWORD dwFlags = WIN_MNT_NOWAIT;
	CHAR szMessage[MAX_MESSAGE];

	if (!vfs_setvfs(&cfData, dwFlags)){
		return;
	}else while (vfs_getvfs(&cfData, dwFlags)){
		if (cfData.FSType == FS_TYPE_PDO){
			pdo_statvfs(&cfData, dwFlags, &cfDriver);
			if (pdo_match(cfData.NtName, cfDriver.DeviceType, &cfDriver)){
				cfmessage(&cfData, &cfDriver, szMessage);
//				msvc_printf(szMessage);
			}else if (!cfDriver.Flags){
				cfmessage(&cfData, &cfDriver, szMessage);
				msvc_printf(szMessage);
			}
		}
	}
	vfs_endvfs(&cfData);
}
