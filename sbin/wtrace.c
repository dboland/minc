#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "win/windows.h"
#include "win_posix.h"
#include "vfs_posix.h"
#include "bsd_posix.h"

#include "libtrace.h"

VOID win_ktrace(STRUCT_TYPE Type, LONG Size, PVOID Data);

/************************************************************/

void 
print_usage(void)
{
	printf("\nPerform ACL trace on various Windows objects.\n\n");
	printf("Usage: %s [options]\n", __progname);
	printf("\nOptions\n");
	printf(" -P\t\t\t%s\n", "print Process token");
	printf(" -T\t\t\t%s\n", "print Thread token (if any)");
	printf(" -c ACCOUNT\t\t%s\n", "print capabilities of ACCOUNT");
	printf(" -f PATH\t\t%s\n", "print ACL file/directory");
	printf(" -p\t\t\t%s\n", "print ACL Process");
	printf(" -d\t\t\t%s\n", "print ACL Desktop");
	printf(" -s\t\t\t%s\n", "print ACL Window Station");
}

/************************************************************/

int 
main(int argc, char* argv[])
{
	WIN_NAMEIDATA wPath = {0};
	char *prog = *argv++;
	char *token = "";
	char *cmd = NULL;
	char *arg = NULL;

	while (arg = *argv++){
		if (!strcmp(arg, "-o")){
			argv++;

		}else if (arg[0] == '-'){
			cmd = arg;

		}else{
			token = arg;

		}
	}
	if (!cmd)
		print_usage();
	else switch(cmd[1]){
		case 'd':
			win_ktrace(STRUCT_ACL_DESKTOP, 0x1000, token);
			break;
		case 's':
			win_ktrace(STRUCT_ACL_STATION, 0x2000, token);
			break;
		case 'p':
			win_ktrace(STRUCT_ACL_PROCESS, 0x0800, token);
			break;
		case 'P':
			win_ktrace(STRUCT_TOKEN_PROCESS, 0x1000, token);
			break;
		case 'T':
			win_ktrace(STRUCT_TOKEN_THREAD, 0x1000, token);
			break;
		case 'f':
			win_ktrace(STRUCT_ACL_FILE, 0x0800, path_win(&wPath, token, O_NOFOLLOW)->Resolved);
			break;
		case 'o':
			win_ktrace(STRUCT_ACL_OBJECT, 0x1000, token);
			break;
		case 'c':
			win_ktrace(STRUCT_SID_RIGHTS, 0x0800, token);
			break;
		default:
			printf("%s: No such option.\n", cmd);
	}
}
