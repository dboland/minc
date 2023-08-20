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

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>
#include <ctype.h>
#include <errno.h>
#include <wchar.h>

#include <sys/stat.h>
#include <sys/systm.h>
#include <sys/mntent.h>

#include "win/windows.h"
#include "win/winsock2.h"
#include "win/iphlpapi.h"
#include "win/aclapi.h"
#include "win_posix.h"
#include "vfs_posix.h"
#include "ws2_posix.h"
#include "minc_posix.h"
#include "msvc_posix.h"

extern __import SID8 SidSystem;
extern __import SID8 SidService;
extern __import SID8 SidAdmins;
extern __import SID8 SidPackageRestrict;

#define WIN_ERR			msvc_printf
#define MAX_PWDBUF		WIN_PATH_MAX

char 	_PWDBUF[MAX_PWDBUF];

int _verbose;
int _paths;

/****************************************************/

void 
print_fsent(WIN_MOUNT *Mount, WIN_STATFS *Stat, LPCWSTR TypeName, DWORD Flags)
{
	char path[MAXPATHLEN];
	char line[255], *l = line;
	WIN_DEVICE *pDevice = drive_match(Mount->NtName, Mount->DeviceType);

	l += sprintf(l, "/dev/%s", pDevice->Name);
	l += sprintf(l, "\t%s", path_posix(path, win_wcslcase((LPWSTR)Stat->Drive)));

	if (!wcsncmp(TypeName, L"FAT", 3)){
		l += sprintf(l, "\tmsdos");
	}else if (!wcscmp(TypeName, L"ISO9660")){
		l += sprintf(l, "\tcd9660");
	}else{
		l += sprintf(l, "\t%ls", win_wcslcase((LPWSTR)TypeName));
	}
	if (Flags & FILE_READ_ONLY_VOLUME){
		l += sprintf(l, "\tro");
	}else{
		l += sprintf(l, "\trw");
	}
	switch (Mount->DriveType){
		case DRIVE_REMOVABLE:
		case DRIVE_CDROM:
			l += sprintf(l, ",noauto");
	}
	if (!(Flags & FILE_PERSISTENT_ACLS)){
		l += sprintf(l, ",nosuid");
	}

	printf("%s\t0\t0\n", line);	/* frequency, passno (2=raw access) */

	if (_paths){
		mkdir(path, 00700);
	}
}
void 
print_usage(char *prog)
{
	printf("Usage: %s [options] COMMAND\n", prog);
	printf("\nCommands\n");
	printf(" passwd\t\t\tprint entries for /etc/master.passwd\n");
	printf(" group\t\t\tprint entries for /etc/group\n");
	printf(" fstab\t\t\tprint entries for /etc/fstab\n");
	printf(" resolv\t\t\tprint entries for /etc/resolv.conf\n");
	printf(" pdo\t\t\tWindows PDO device table (physical drivers)\n");
	printf(" fdo\t\t\tWindows FDO device table (function drivers)\n");
	printf(" vol\t\t\tWindows volume table\n");
	printf(" link\t\t\tWindows link table\n");
	printf(" drive\t\t\tWindows drive table\n");
	printf(" npf\t\t\tNetGroup Packet Filter table\n");
	printf(" ndis\t\t\tWindows network driver table\n");
	printf(" if\t\t\tWindows network adapter table\n");
	printf("\nOptions\n");
	printf(" -p, --path\t\tcreate paths while printing\n");
	printf(" -v, --verbose\t\tbe verbose if applicable\n");
}

/****************************************************/

void 
mk_group(FILE *stream)
{
	WIN_GRENT wGroup;
	char buf[MAX_TEXT] = "";
	WIN_PWENUM wnInfo = {0};
	DWORD wksType = 0;
	DWORD dwSize;
	SID8 sidBuf;

	while (wksType++ < WinWellKnownSidTypeCount){
		dwSize = sizeof(SID8);
		if (CreateWellKnownSid(wksType, NULL, &sidBuf, &dwSize)){
			if (sidBuf.SubAuthority[0] == SECURITY_BUILTIN_DOMAIN){	/* 32 (Builtin) */
				continue;
			}else if (!win_getgrgid(&sidBuf, &wGroup)){
				continue;
			}
		}else if (wksType != WinApplicationPackageAuthoritySid){	/* NT */
			continue;
		}else if (!win_getgrgid(&SidPackageRestrict, &wGroup)){	/* Vista */
			continue;
		}
		fprintf(stream, "%s\n", group_posix(buf, MAX_TEXT, &wGroup));
	}
	win_setgrent(&wnInfo, WIN_NETENUM_LOCAL);
	while (win_getgrent(&wnInfo, &wGroup)){
		fprintf(stream, "%s\n", group_posix(buf, MAX_TEXT, &wGroup));
	}
	win_endgrent(&wnInfo);
}
void 
mk_passwd(FILE *stream)
{
	WIN_PWENT pwEntry = {0};
	WIN_PWENUM pwEnum = {0};

	win_getpwuid(&SidSystem, &pwEntry);
	fprintf(stream, "%s\n", passwd_posix(_PWDBUF, MAX_PWDBUF, &pwEntry));
	win_getpwuid(&SidService, &pwEntry);
	fprintf(stream, "%s\n", passwd_posix(_PWDBUF, MAX_PWDBUF, &pwEntry));
	if (win_getpwnam(L"NT SERVICE\\TrustedInstaller", &pwEntry)){	/* Vista */
		fprintf(stream, "%s\n", passwd_posix(_PWDBUF, MAX_PWDBUF, &pwEntry));
	}
	win_setpwent(&pwEnum, WIN_NETENUM_LOCAL);
	while (win_getpwent(&pwEnum, &pwEntry)){
		fprintf(stream, "%s\n", passwd_posix(_PWDBUF, MAX_PWDBUF, &pwEntry));
//		if (_paths){
//			mkdir(path, 00700);
//		}
	}
	win_endpwent(&pwEnum);
}
void 
mk_resolv(FILE *stream)
{
	ULONG ulSize = 0;
	DWORD dwStatus;
	FIXED_INFO *fInfo;
	DWORD dwIndex;
	IP_ADDR_STRING *ipEntry;

	dwStatus = GetNetworkParams(NULL, &ulSize);
	if (ulSize){
		fInfo = win_malloc(ulSize);
		GetNetworkParams(fInfo, &ulSize);
		fprintf(stream, "domain %s\n", fInfo->DomainName);
		for (ipEntry = &fInfo->DnsServerList; ipEntry; ipEntry = ipEntry->Next){
			fprintf(stream, "nameserver %s\n", ipEntry->IpAddress.String);
		}
		win_free(fInfo);
	}else{
		WIN_ERR("GetNetworkParams(): %s\n", win_strerror(dwStatus));
	}
}
int 
mk_fstab(FILE *stream)
{
	int result = 0;
	WIN_MOUNT wMount;
	WIN_CFDATA cfData;
	DWORD dwFlags = WIN_MNT_NOWAIT | WIN_MNT_REVERSED;
	WIN_STATFS wsInfo;

	if (!vfs_setvfs(&cfData, dwFlags)){
		fprintf(stderr, "vol_setfsstat(): %s\n", win_strerror(errno_win()));
	}else while (vfs_getvfs(&cfData, dwFlags)){
		if (cfData.FSType == FS_TYPE_DRIVE){
			if (!drive_statvfs(&cfData, dwFlags, &wMount)){
				drive_match(wMount.NtName, wMount.DeviceType);
			}else if (drive_getfsstat(&wMount, dwFlags, &wsInfo)){
				print_fsent(&wMount, &wsInfo, wsInfo.TypeName, wsInfo.Flags);
			}
		}
	}
	vfs_endvfs(&cfData);
	return(result);
}
void 
mk_fsent(WIN_FS_TYPE Type)
{
	WIN_CFDATA fsEnum;
	char buf[PATH_MAX] = "";

	if (!vfs_setvfs(&fsEnum, 0)){
		fprintf(stderr, "vfs_setfsstat(): %s\n", strerror(errno));
	}else while (vfs_getvfs(&fsEnum, 0)){
		if (fsEnum.FSType == Type){
			if (_verbose){
				printf("%ls: %s\n", fsEnum.DosPath, path_posix(buf, fsEnum.NtPath));
			}else{
				printf("%ls: %s\n", fsEnum.BusName, path_posix(buf, fsEnum.NtPath));
			}
		}
	}
	vfs_endvfs(&fsEnum);
}
void 
mk_vol(FILE *stream)
{
	WIN_CFDATA cfData;
	CHAR szMessage[MAX_MESSAGE];
	char buf[PATH_MAX];

	if (!vfs_setvfs(&cfData, 0)){
		fprintf(stderr, "vfs_setfsstat(): %s\n", strerror(errno));
	}else while (vfs_getvfs(&cfData, 0)){
		if (cfData.FSType == FS_TYPE_VOLUME){
			printf("%ls: %s\n", cfData.DosPath, path_posix(buf, cfData.NtPath));
			if (!vol_stat(cfData.DosPath, szMessage)){
				printf("   %s\n", win_strerror(errno_win()));
			}else{
				printf("   %s\n", szMessage);
			}
		}
	}
	vfs_endvfs(&cfData);
}
void 
mk_ifent(WIN_FS_TYPE Type)
{
	PMIB_IFROW pifRow;
	DWORD dwCount = 0;
	WIN_IFDATA ifData;

	if (!ws2_setvfs(&ifData, TRUE, &pifRow, &dwCount)){
		fprintf(stderr, "ws_NET_RT_IFLIST(): %s\n", strerror(errno));
	}else while (dwCount--){
		ws2_statvfs(&ifData, pifRow);
		if (ifData.FSType == Type){
			printf("%ls: [%03d] %s\n", ifData.NtName, pifRow->dwIndex, pifRow->bDescr);
		}
		pifRow++;
	}
	ws2_endvfs(&ifData);
}

/****************************************************/

int 
main(int argc, char *argv[])
{
	char *cmd = NULL;
	char *token;
	char *progname = *argv++;
	int result = 0;

	while (token = *argv){
		if (!strcmp(token, "-v")){
			_verbose++;
		}else if (!strcmp(token, "-p")){
			_paths++;
		}else if (token[0] != '-'){
			cmd = token;
		}
		argv++;
	}
	if (!cmd){
		print_usage(progname);
	}else if (!strcmp(cmd, "group")){
		mk_group(stdout);
	}else if (!strcmp(cmd, "passwd")){
		mk_passwd(stdout);
	}else if (!strcmp(cmd, "fstab")){
		mk_fstab(stdout);
	}else if (!strcmp(cmd, "resolv")){
		mk_resolv(stdout);
	}else if (!strcmp(cmd, "pdo")){
		mk_fsent(FS_TYPE_DEVICE);
	}else if (!strcmp(cmd, "vol")){
		mk_vol(stdout);
	}else if (!strcmp(cmd, "fdo")){
		mk_fsent(FS_TYPE_PROCESS);
	}else if (!strcmp(cmd, "link")){
		mk_fsent(FS_TYPE_LINK);
	}else if (!strcmp(cmd, "drive")){
		mk_fsent(FS_TYPE_DRIVE);
	}else if (!strcmp(cmd, "npf")){
		mk_fsent(FS_TYPE_NPF);
	}else if (!strcmp(cmd, "if")){
		mk_ifent(FS_TYPE_WINSOCK);
	}else if (!strcmp(cmd, "ndis")){
		mk_ifent(FS_TYPE_NDIS);
	}else{
		printf("%s: no such entity.\n", cmd);
		result = -1;
	}
	return(result);
}
