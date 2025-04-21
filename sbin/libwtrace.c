#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <aclapi.h>
#include <accctrl.h>

#include <ddk/ntapi.h>
#include <ddk/ntifs.h>	// Object types

#include <stdio.h>

#include "win_types.h"
#include "dev_types.h"
#include "vfs_types.h"

#include "libtrace.h"

LPSTR win_strerror(HRESULT Error);

#define WIN_ERR		printf

/************************************************************/

HANDLE 
GetProcessHandle(DWORD ProcessId)
{
	HANDLE hResult = NULL;

	if (!ProcessId){
		hResult = GetCurrentProcess();
	}else if (!(hResult = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId))){
		WIN_ERR("OpenProcess(%d): %s\n", ProcessId, win_strerror(GetLastError()));
	}
	return(hResult);
}
HANDLE 
GetThreadHandle(DWORD ThreadId)
{
	HANDLE hResult = NULL;

	if (!ThreadId){
		hResult = GetCurrentThread();
	}else if (!(hResult = OpenThread(THREAD_ALL_ACCESS, FALSE, ThreadId))){
		WIN_ERR("OpenThread(%d): %s\n", ThreadId, win_strerror(GetLastError()));
	}
	return(hResult);
}

/************************************************************/

VOID 
print_acl_desktop(VOID)
{
	PACL Acl = NULL;
	PSECURITY_DESCRIPTOR Sd;
	HDESK hObject = OpenInputDesktop(0, FALSE, READ_CONTROL | DESKTOP_READOBJECTS);
	CHAR szBuffer[4096];

	GetSecurityInfo(hObject, SE_WINDOW_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &Acl, NULL, &Sd);
	if (Acl){
		win_ACL(szBuffer, "ACL Desktop", Acl, OB_TYPE_DESKTOP);
		printf(szBuffer);
		LocalFree(Sd);
	}else{
		WIN_ERR("GetSecurityInfo(%d): %s\n", hObject, win_strerror(GetLastError()));
	}
	CloseDesktop(hObject);
}
VOID 
print_acl_station(VOID)
{
	PACL Acl = NULL;
	PSECURITY_DESCRIPTOR Sd;
	HWINSTA hObject = GetProcessWindowStation();
	CHAR szBuffer[4096];

	GetSecurityInfo(hObject, SE_WINDOW_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &Acl, NULL, &Sd);
	if (Acl){
		win_ACL(szBuffer, "ACL WindowStation", Acl, OB_TYPE_WINDOW_STATION);
		printf(szBuffer);
		LocalFree(Sd);
	}else{
		WIN_ERR("GetSecurityInfo(%d): %s\n", hObject, win_strerror(GetLastError()));
	}
}
VOID 
print_acl_process(VOID)
{
	DWORD dwType = OWNER_SECURITY_INFORMATION + GROUP_SECURITY_INFORMATION + DACL_SECURITY_INFORMATION;
	PACL Acl = NULL;
	PSECURITY_DESCRIPTOR psd;
	DWORD dwSize = 0;
	HANDLE hProcess = GetCurrentProcess();
	CHAR szBuffer[4096];

	GetUserObjectSecurity(hProcess, &dwType, NULL, dwSize, &dwSize);
	psd = LocalAlloc(LMEM_FIXED, dwSize);
	if (!GetUserObjectSecurity(hProcess, &dwType, psd, dwSize, &dwSize)){
		WIN_ERR("GetUserObjectSecurity(%d): %s\n", hProcess, win_strerror(GetLastError()));
	}else{
		win_SECURITY_DESCRIPTOR(psd, OB_TYPE_PROCESS, szBuffer);
		printf(szBuffer);
	}
	LocalFree(psd);
}
VOID 
print_acl_file(LPWSTR FileName)
{
	DWORD dwType = OWNER_SECURITY_INFORMATION + GROUP_SECURITY_INFORMATION + DACL_SECURITY_INFORMATION;
	PSECURITY_DESCRIPTOR psd;
	DWORD dwSize = 0;
	BOOL bDefaulted;
	BOOL bPresent;
	PACL Acl;
	CHAR szBuffer[4096];

	GetFileSecurityW(FileName, dwType, NULL, dwSize, &dwSize);
	psd = LocalAlloc(LMEM_FIXED, dwSize);
	if (!GetFileSecurityW(FileName, dwType, psd, dwSize, &dwSize)){
		WIN_ERR("GetFileSecurity(%ls): %s\n", FileName, win_strerror(GetLastError()));
	}else{
		printf("FileName: %ls\n", FileName);
		win_SECURITY_DESCRIPTOR(psd, OB_TYPE_PROCESS, szBuffer);
		printf(szBuffer);
	}
	LocalFree(psd);
}
VOID 
print_acl_object(LPCSTR Name)
{
	DWORD dwType = OWNER_SECURITY_INFORMATION + GROUP_SECURITY_INFORMATION + DACL_SECURITY_INFORMATION;
	PSECURITY_DESCRIPTOR psd;
	DWORD dwSize = 0;
	BOOL bDefaulted;
	BOOL bPresent;
	PACL Acl;
	CHAR szBuffer[4096];

	GetFileSecurity(Name, dwType, NULL, dwSize, &dwSize);
	psd = LocalAlloc(LMEM_FIXED, dwSize);
	if (!GetFileSecurity(Name, dwType, psd, dwSize, &dwSize)){
		WIN_ERR("GetFileSecurity(%s): %s\n", Name, win_strerror(GetLastError()));
	}else{
		printf("Resolved: %s\n", Name);
		win_SECURITY_DESCRIPTOR(psd, OB_TYPE_PROCESS, szBuffer);
		printf(szBuffer);
	}
	LocalFree(psd);
}
VOID 
print_process_token(DWORD ProcessId)
{
	HANDLE hToken;
	CHAR szBuffer[4096];

	if (!OpenProcessToken(GetProcessHandle(ProcessId), MAXIMUM_ALLOWED, &hToken)){
		WIN_ERR("GetCurrentProcess(%d): %s\n", ProcessId, win_strerror(GetLastError()));
	}else{
		win_TOKEN(hToken, szBuffer);
		printf(szBuffer);
		CloseHandle(hToken);
	}
}
VOID 
print_thread_token(DWORD ThreadId)
{
	HANDLE hToken = NULL;
	CHAR szBuffer[4096];

	if (!OpenThreadToken(GetThreadHandle(ThreadId), MAXIMUM_ALLOWED, TRUE, &hToken)){
		WIN_ERR("OpenThreadToken(%d): %s\n", ThreadId, win_strerror(GetLastError()));
	}else{
		win_TOKEN(hToken, szBuffer);
		printf(szBuffer);
		CloseHandle(hToken);
	}
}
