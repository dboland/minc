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

#if __i386__
typedef DWORD TLSINT;
#else
typedef ULONGLONG TLSINT;
#endif

/************************************************************/

VOID 
win_tls_attach(const IMAGE_TLS_DIRECTORY *Image)
{
	DWORD dwSize = Image->EndAddressOfRawData - Image->StartAddressOfRawData;
	DWORD dwIndex = *(TLSINT *)Image->AddressOfIndex;

	__Offsets[dwIndex].Size = dwSize;
	__Offsets[dwIndex].Address = (LPBYTE)Image->StartAddressOfRawData;
}
/* must be removed, but still used by gcc.exe */
LPBYTE 
win_tls_get(LPBYTE Data[], const IMAGE_TLS_DIRECTORY *Image, const LPBYTE Address)
{
	DWORD dwIndex = *(TLSINT *)Image->AddressOfIndex;
	DWORD dwOffset = Address - (LPBYTE)Image->StartAddressOfRawData;

	return(Data[dwIndex] + dwOffset);
}
