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

#include <winnls.h>

/************************************************************/

INT 
win_mbstowcs(LPWSTR Destination, LPCSTR Source, INT Size)
{
	INT iResult;

	if (!Destination) Size = 0;
	iResult = MultiByteToWideChar(CP_UTF8, 0, Source, -1, Destination, Size);
	return(iResult - 1);
}
LPWSTR 
win_mbstowcp(LPWSTR Destination, LPCSTR Source, INT Size)
{
	INT iResult;

	iResult = MultiByteToWideChar(CP_UTF8, 0, Source, -1, Destination, Size);
	return(Destination + iResult - 1);
}
INT 
win_wcstombs(LPSTR Destination, LPCWSTR Source, INT Size)
{
	INT iResult;

	if (!Destination) Size = 0;
	iResult = WideCharToMultiByte(CP_UTF8, 0, Source, -1, Destination, Size, NULL, NULL);
	return(iResult - 1);
}
LPSTR 
win_wcstombp(LPSTR Destination, LPCWSTR Source, INT Size)
{
	INT iResult;

	iResult = WideCharToMultiByte(CP_UTF8, 0, Source, -1, Destination, Size, NULL, NULL);
	return(Destination + iResult - 1);
}
LPWSTR 
win_wcscpy(LPWSTR Destination, LPCWSTR Source)
{
	LPWSTR wszResult = Destination;

	while (*Destination = *Source++){
		Destination++;
	}
	return(wszResult);
}
LPWSTR 
win_wcpcpy(LPWSTR Destination, LPCWSTR Source)
{
	while (*Destination = *Source++){
		Destination++;
	}
	return(Destination);
}
LPWSTR 
win_wcscat(LPWSTR Destination, LPCWSTR Source)
{
	LPWSTR pszResult = Destination;

	while (*Destination){
		Destination++;
	}
	win_wcscpy(Destination, Source);
	return(pszResult);
}
INT 
win_wcslen(LPCWSTR String)
{
	LONG lResult = 0;

	while (*String++){
		lResult++;
	}
	return(lResult);
}
INT 
win_wcsncmp(LPCWSTR String1, LPCWSTR String2, LONG Length)
{
	return(CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNOREWIDTH, String1, Length, String2, Length) - 2);
}
INT 
win_wcscmp(LPCWSTR String1, LPCWSTR String2)
{
	return(CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNOREWIDTH, String1, -1, String2, -1) - 2);
}
LPWSTR 
win_wcsncpy(LPWSTR Destination, LPCWSTR Source, LONG Length)
{
	LPWSTR pszResult = Destination;
	WCHAR C;

	while (C = *Source++){
		Length--;
		if (Length < 1){
			break;
		}
		*Destination++ = C;
	}
	*Destination = 0;
	return(pszResult);
}
LPWSTR 
win_wcslcase(LPWSTR String)
{
	return(CharLowerW(String));
}
LPWSTR 
win_wcsucase(LPWSTR String)
{
	return(CharUpperW(String));
}
