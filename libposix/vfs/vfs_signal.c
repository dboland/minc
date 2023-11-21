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

/************************************************************/

LONG CALLBACK 
SigExceptionProc(PEXCEPTION_POINTERS ExceptionInfo)
{
	PEXCEPTION_RECORD peRecord = ExceptionInfo->ExceptionRecord;
	PCONTEXT pContext = ExceptionInfo->ContextRecord;
	DWORD dwExceptionCode = peRecord->ExceptionCode;
	MEMORY_BASIC_INFORMATION mbInfo;
	WCHAR szModule[MAX_PATH];
	DWORD dwCtrlType = 0;
	LONG lResult = EXCEPTION_EXECUTE_HANDLER;

	switch (dwExceptionCode){
		case STATUS_ACCESS_VIOLATION:
			dwCtrlType = CTRL_ACCESS_VIOLATION_EVENT;
			break;
		case STATUS_ILLEGAL_INSTRUCTION:
		case STATUS_PRIVILEGED_INSTRUCTION:
		case STATUS_INVALID_UNWIND_TARGET:
			dwCtrlType = CTRL_ILLEGAL_INSTRUCTION_EVENT;
			break;
		case STATUS_FLOAT_DIVIDE_BY_ZERO:
		case STATUS_INTEGER_DIVIDE_BY_ZERO:
			dwCtrlType = CTRL_DIVIDE_BY_ZERO_EVENT;
			break;
		case STATUS_STACK_OVERFLOW:
			msvc_printf("Stack overflow\n");
			dwCtrlType = CTRL_ABORT_EVENT;
			break;
		default:
			msvc_printf("ExceptionProc(0x%x) %s: Not implemented.\n", dwExceptionCode, nt_strerror(GetLastError()));
	}
	if (win_dladdr(peRecord->ExceptionAddress, &mbInfo, szModule)){
		WIN_ERR("Exception(0x%X) in %ls at 0x%x\n", dwExceptionCode, szModule, pContext->Eip);
	}
	if (__SignalProc(dwCtrlType, pContext)){
		lResult = EXCEPTION_CONTINUE_SEARCH;	/* httpd.exe */
	}
	return(lResult);
}

/************************************************************/

BOOL 
vfs_kill_PID(DWORD ThreadId, UINT Message, WPARAM WParam, LPARAM LParam)
{
	BOOL bResult = FALSE;

	if (!PostThreadMessage(ThreadId, Message, WParam, LParam)){
//		WIN_ERR("PostThreadMessage(%d): %s\n", ThreadId, win_strerror(GetLastError()));
		return(FALSE);	/* child orphaned */
	}else if (!SetEvent(__Interrupt)){
		WIN_ERR("SetEvent(%d): %s\n", __Interrupt, win_strerror(GetLastError()));
	}else{
		bResult = TRUE;
	}
	return(bResult);
}
BOOL 
vfs_kill_GRP(DWORD GroupId, UINT Message, WPARAM WParam, LPARAM LParam)
{
	BOOL bResult = TRUE;
	DWORD dwIndex = WIN_CHILD_MAX - 1;
	WIN_TASK *pwTask = &__Tasks[dwIndex];

	while (dwIndex > WIN_PID_INIT){
		if (pwTask->GroupId == GroupId){
			vfs_kill_PID(pwTask->ThreadId, Message, WParam, LParam);
		}
		dwIndex--;
		pwTask--;
	}
	return(bResult);
}
BOOL 
vfs_kill_ANY(DWORD ParentId, UINT Message, WPARAM WParam, LPARAM LParam)
{
	BOOL bResult = TRUE;
	DWORD dwIndex = WIN_CHILD_MAX - 1;
	WIN_TASK *pwTask = &__Tasks[dwIndex];

	while (dwIndex > WIN_PID_INIT){
		if (pwTask->ParentId == ParentId){
			vfs_kill_PID(pwTask->ThreadId, Message, WParam, LParam);
		}
		dwIndex--;
		pwTask--;
	}
	return(bResult);
}
BOOL 
vfs_kill_SYS(DWORD CallerId, UINT Message, WPARAM WParam, LPARAM LParam)
{
	BOOL bResult = TRUE;
	DWORD dwIndex = WIN_PID_INIT;
	WIN_TASK *pwTask = &__Tasks[dwIndex];

	if (!win_group_member(&SidAdmins)){
		SetLastError(ERROR_PRIVILEGE_NOT_HELD);
		bResult = FALSE;
	}else while (dwIndex < WIN_CHILD_MAX){
		if (pwTask->Flags && pwTask->TaskId != CallerId){
			vfs_kill_PID(pwTask->ThreadId, Message, WParam, LParam);
		}
		dwIndex++;
		pwTask++;
	}
	return(bResult);
}

/************************************************************/

BOOL 
vfs_raise(UINT Message, WPARAM WParam, LPARAM LParam)
{
	DWORD CtrlType;
	CONTEXT ctx = {0};

	switch (Message){
		case WM_SIZE:
			CtrlType = CTRL_SIZE_EVENT;
			break;
		case WM_CLOSE:
			CtrlType = CTRL_CLOSE_EVENT;
			break;
		case WM_QUIT:
			CtrlType = CTRL_QUIT_EVENT;
			break;
		case WM_COMMAND:
			CtrlType = WParam;
			break;
		case WM_TIMER:
			CtrlType = CTRL_TIMER_EVENT;
			break;
		case WM_USER:
			CtrlType = CTRL_DETACH_EVENT;
			break;
		default:
			/* WM_STRING class message from WinXP, when started
			 * with START batch command
			 */
			CtrlType = CTRL_INFO_EVENT;
	}
	return(__SignalProc(CtrlType, &ctx));
}
BOOL 
vfs_sigsuspend(WIN_TASK *Task, CONST UINT *Mask, WPARAM WParam, MSG *Result)
{
	HACCEL hKeys = NULL;
	BOOL bResult = TRUE;
	UINT uiCurrent = Task->ProcMask;

	Task->ProcMask = *Mask;
	Task->State = WIN_SSLEEP;
	if (GetMessage(Result, NULL, 0, 0)){
		if (!TranslateAccelerator(NULL, hKeys, Result)){
			TranslateMessage(Result);
			DispatchMessage(Result);
		}
	}
	if (Result->wParam == WParam){	/* vfs_nanosleep() */
		bResult = FALSE;
	}else if (vfs_raise(Result->message, Result->wParam, Result->lParam)){
		SetLastError(ERROR_SIGNAL_PENDING);
	}else{
		bResult = FALSE;
	}
	Task->State = WIN_SRUN;
	Task->ProcMask = uiCurrent;
	return(bResult);
}
