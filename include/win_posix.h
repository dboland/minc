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

#include "win_types.h"

/* win_string.c */

LPSTR win_strerror(HRESULT Error);
LPSTR nt_strerror(NTSTATUS Status);
LPSTR win_strcpy(LPSTR Destination, LPCSTR Source);
LPSTR win_stpcpy(LPSTR Destination, LPCSTR Source);
LPSTR win_strncpy(LPSTR Destination, LPCSTR Source, DWORD Length);
INT win_strlen(LPCSTR String);
VOID win_memcpy(LPVOID Destination, LPCVOID Source, DWORD Size);
INT win_strcmp(LPCSTR String1, LPCSTR String2);
INT win_strncmp(LPCSTR String1, LPCSTR String2, LONG Length);
LPCSTR win_index(LPCSTR String, CHAR Chr);
VOID win_memset(PVOID Dest, CHAR Fill, DWORD Size);
LPSTR win_strcat(LPSTR Destination, LPCSTR Source);
VOID win_bzero(LPVOID Buffer, DWORD Size);
LPSTR win_strlcase(LPSTR String);
DWORD win_strtime(SYSTEMTIME *Time, LPSTR Buffer, DWORD Size);
LPSTR win_strsid(SID8 *Sid);
LPSTR win_strobj(HANDLE Objects[], DWORD Count);
BOOL win_isu8cont(UCHAR C);

/* win_wchar.c */

INT win_mbstowcs(LPWSTR Destination, LPCSTR Source, int Bufsize);
LPWSTR win_mbstowcp(LPWSTR Destination, LPCSTR Source, INT Bufsize);
INT win_wcstombs(LPSTR Destination, LPCWSTR Source, int Bufsize);
LPSTR win_wcstombp(LPSTR Destination, LPCWSTR Source, int Bufsize);
LPWSTR win_wcscpy(LPWSTR Destination, LPCWSTR Source);
LPWSTR win_wcpcpy(LPWSTR Destination, LPCWSTR Source);
LPWSTR win_wcscat(LPWSTR Destination, LPCWSTR Source);
INT win_wcsncmp(LPCWSTR String1, LPCWSTR String2, LONG Length);
INT win_wcscmp(LPCWSTR String1, LPCWSTR String2);
LPWSTR win_wcsncpy(LPWSTR Destination, LPCWSTR Source, DWORD Length);
LPWSTR win_wcslcase(LPWSTR String);
INT win_wcslen(LPCWSTR String);
LPWSTR win_wcsucase(LPWSTR String);

/* win_libgen.c */

LPWSTR win_dirname(LPWSTR FileName);
LPWSTR win_dirname_r(LPCWSTR FileName, LPWSTR DirName);
LPWSTR win_basename(LPCWSTR FileName);
LPWSTR win_typename(LPCWSTR FileName);
LPWSTR win_drivename(LPCWSTR FileName, LPWSTR Drive);
BOOL win_flagname(DWORD Flag, LPCSTR Name, DWORD Mask, DWORD *Remain);
LPWSTR win_docname_r(LPCWSTR FileName, LPWSTR Buffer, LONG Size);
BOOL win_sidname(SID8 *Sid, LPSTR Buffer, DWORD Length);

/* win_stdlib.c */

PVOID win_malloc(UINT Size);
PVOID win_realloc(PVOID Buffer, UINT Size);
VOID win_free(PVOID Buffer);
DWORD win_getenv(LPCSTR Name, LPCSTR Buffer, DWORD Size);

/* win_acl.c */

VOID win_acl_PROCESS_ATTACH(VOID);
BOOL win_acl_get_file(LPCWSTR FileName, PSECURITY_DESCRIPTOR *Result);
BOOL win_acl_get_fd(HANDLE Handle, PSECURITY_DESCRIPTOR *Result);
BOOL win_acl_init(WIN_MODE *Mode, WIN_ACL_CONTROL *Result);
BOOL win_acl_dup(PSECURITY_DESCRIPTOR Security, WIN_ACL_CONTROL *Result);
VOID win_acl_free(WIN_ACL_CONTROL *Control);

/* win_unistd.c */

SID8 *win_getuid(SID8 *Sid);
SID8 *win_geteuid(SID8 *Sid);
SID8 *win_getgid(SID8 *Sid);
SID8 *win_getegid(SID8 *Sid);
BOOL win_group_member(PSID Group);
BOOL win_getgroups(SID8 **Groups, DWORD *Count);
BOOL win_setgroups(SID8 Groups[], DWORD Count);
BOOL win___tfork_thread(WIN___TFORK *Params, SIZE_T Size, LPTHREAD_START_ROUTINE *Start, PVOID Data, DWORD *Result);

/* win_pwd.c */

BOOL win_getpwnam(LPCWSTR Account, WIN_PWENT *Entity);
BOOL win_getpwuid(SID8 *Sid, WIN_PWENT *Entity);
BOOL win_setpwent(WIN_PWENUM *Info, DWORD Type);
BOOL win_getpwent(WIN_PWENUM *Info, WIN_PWENT *Result);
VOID win_endpwent(WIN_PWENUM *Info);

/* win_grp.c */

BOOL win_getgrnam(LPCWSTR Name, WIN_GRENT *Group);
BOOL win_getgrgid(SID8 *Sid, WIN_GRENT *Group);
BOOL win_setgrent(WIN_PWENUM *Info, DWORD Type);
BOOL win_getgrent(WIN_PWENUM *Info, WIN_GRENT *Group);
VOID win_endgrent(WIN_PWENUM *Info);
BOOL win_getgrouplist(WIN_PWENT *Passwd, SID8 *Primary, SID8 *Result[], DWORD *Count);

/* win_fcntl.c */

HANDLE win_F_DUPFD(HANDLE Handle, BOOL Inherited);
BOOL win_F_SETFD(HANDLE Handle, DWORD Info);
LPWSTR win_F_GETPATH(HANDLE Handle, LPWSTR Buffer);
ACCESS_MASK win_F_GETFL(HANDLE Handle);
DWORD win_F_GETFD(HANDLE Handle);
HANDLE win_F_DISINHERIT(HANDLE Handle, DWORD ProcessId);
HANDLE win_F_INHERIT(HANDLE Handle, ACCESS_MASK Access, DWORD ProcessId);

/* vfs_sysctl.c */

BOOL win_KERN_CLOCKRATE(DWORDLONG *Result);
BOOL win_KERN_HOSTNAME(LPSTR Current, LPCSTR New, DWORD Size);
BOOL win_KERN_DOMAINNAME(LPSTR Current, LPCSTR New, DWORD Size);
BOOL win_KERN_PROC(DWORD ThreadId, WIN_KINFO_PROC *Result);
BOOL win_KERN_TIMECOUNTER_TICK(DWORDLONG *Result);

/* win_mman.c */

BOOL win_mmap(HANDLE Handle, PVOID Address, SIZE_T Size, LARGE_INTEGER *Offset, DWORD Protect, PVOID *Result);
BOOL win_munmap(PVOID Address, SIZE_T Size);
BOOL win_mprotect(PVOID Address, SIZE_T Size, DWORD Protect);
BOOL win_madvise(PVOID Address, SIZE_T Size, DWORD Type);

/* win_capability.c */

BOOL win_cap_get_proc(DWORD Access, TOKEN_TYPE Type, HANDLE *Result);
BOOL win_cap_set_mode(SID8 *User, ACCESS_MASK Access, ACL *Result);
BOOL win_cap_setuid(WIN_PWENT *Passwd, HANDLE *Result);
BOOL win_cap_setgroups(SID8 *Primary, SID8 Groups[], DWORD Count, HANDLE *Result);

/* win_dlfcn.c */

HMODULE win_dlopen(LPCWSTR FileName);
BOOL win_dlclose(HMODULE Module);
FARPROC win_dlsym(HMODULE Module, LPCSTR ProcName);
BOOL win_dladdr(LPCVOID Address, MEMORY_BASIC_INFORMATION *Info, LPWSTR FileName);

/* win_syscall.c */

DECLSPEC_NORETURN VOID win_exit(DWORD ExitCode);
BOOL win_execve(LPSTR Command, LPCSTR Path);

/* win_resource.c */

BOOL win_getrlimit_DATA(WIN_RLIMIT *Limit);
BOOL win_getrlimit_AS(WIN_RLIMIT *Limit);

/* win_ldt.c */

VOID win_ldt_attach(DWORD Count);
BOOL win_ldt_read(HANDLE Handle, DWORD Selector, LPCSTR Label);
BOOL win_ldt_write(DWORD Selector, DWORD Base, DWORD Limit);

/* win_signal.c */

BOOL win_kill(DWORD ThreadId, UINT Message, WPARAM WParam, LPARAM LParam);

/* win_uio.c */

BOOL win_writev(HANDLE Handle, const WIN_IOVEC Data[], DWORD Count, ULONG *Result);

/* win_time.c */

BOOL win_settimeofday(DWORDLONG Time, WORD MilliSeconds);

/* win_mount.c */

BOOL win_mount(LPCWSTR Directory, LPCWSTR Volume);
BOOL win_unmount(LPCWSTR Directory);
