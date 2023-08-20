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

#include <wincon.h>

#define ANSI_PRIOR	"\e[S"	/* kP */
#define ANSI_NEXT	"\e[T"	/* kN */
#define ANSI_END	"\e[F"
#define ANSI_HOME	"\e[H"	/* kh */
#define ANSI_LEFT	"\e[D"	/* kl */
#define ANSI_UP		"\e[A"	/* ku */
#define ANSI_RIGHT	"\e[C"	/* Kr */
#define ANSI_DOWN	"\e[B"	/* kF */
#define ANSI_INSERT	"\e[L"	/* kI */
#define ANSI_DELETE	"\177"	/* kD was: ^[[M */
#define ANSI_F1		"\eF1"
#define ANSI_F2		"\eF2"
#define ANSI_F3		"\eF3"
#define ANSI_F4		"\eF4"
#define ANSI_F5		"\eF5"
#define ANSI_F6		"\eF6"
#define ANSI_F7		"\eF7"
#define ANSI_F8		"\eF8"
#define ANSI_F9		"\eF9"
#define ANSI_F10	"\eFA"
#define ANSI_F11	"\eFB"
#define ANSI_F12	"\eFC"

/* Constants for ScreenAnsi() */

#define ANSI_REP		'_'
#define ANSI_VEM		'b'

#define FOREGROUND_DEFAULT		(FOREGROUND_WHITE)
#define BACKGROUND_DEFAULT		(BACKGROUND_BLACK)
#define FOREGROUND_UNDERLINE		(FOREGROUND_CYAN)

/****************************************************/

BOOL 
AnsiEraseInDisplay(HANDLE Handle, CSB_INFO *Info, CHAR Parm)
{
	DWORD dwScreen;
	DWORD dwCount;
	DWORD dwOffset = 0;
	SMALL_RECT sRect = Info->srWindow;
	COORD cPos = {sRect.Left, sRect.Top};

	if (!Parm || Parm == '0'){
		cPos = Info->dwCursorPosition;
		dwOffset = sRect.Right - cPos.X;
	}else if (Parm == '1'){
		dwOffset = (sRect.Bottom - Info->dwCursorPosition.Y) * ((sRect.Right - sRect.Left) + 1);
		dwOffset += Info->dwCursorPosition.X;
	}else if (Parm == '2'){
		SetConsoleCursorPosition(Handle, cPos);
	}else{
		return(FALSE);
	}
	dwScreen = ((sRect.Bottom - sRect.Top) + 1) * ((sRect.Right - sRect.Left) + 1);
	FillConsoleOutputAttribute(Handle, Info->wAttributes, dwScreen - dwOffset, cPos, &dwCount);
	return(FillConsoleOutputCharacter(Handle, ' ', dwScreen - dwOffset, cPos, &dwCount));
}
BOOL 
AnsiRepeat(HANDLE Handle, CHAR C, SHORT Count)
{
	return(TRUE);
}
BOOL 
AnsiVerticalEditingMode(HANDLE Handle, SHORT Parm)
{
	BOOL bResult = TRUE;

	if (Parm == 1){
		__CTTY->VEdit = TRUE;
	}else if (Parm == 2){
		__CTTY->VEdit = FALSE;
	}else{
		bResult = FALSE;
	}
	return(bResult);
}
