#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "win/windows.h"
#include "win_posix.h"
#include "vfs_posix.h"
#include "bsd_posix.h"

VOID print_process_token(DWORD ProcessId);
VOID print_thread_token(DWORD ThreadId);
VOID print_acl_desktop(VOID);
VOID print_acl_station(VOID);
VOID print_acl_process(VOID);
VOID print_acl_file(LPWSTR FileName);
VOID print_acl_process(VOID);
VOID print_acl_object(LPCSTR Name);

/************************************************************/

void 
print_usage(void)
{
	printf("\nPerform ACL trace on various Windows objects.\n\n");
	printf("Usage: %s [options]\n", __progname);
	printf("\nOptions\n");
	printf(" -P\t\t\t%s\n", "print Process token");
	printf(" -T\t\t\t%s\n", "print Thread token (if any)");
	printf(" -C\t\t\t%s\n", "include capabilities");
	printf(" -f PATH\t\t%s\n", "print ACL file/directory");
	printf(" -p\t\t\t%s\n", "print ACL Process");
	printf(" -d\t\t\t%s\n", "print ACL Desktop");
	printf(" -s\t\t\t%s\n", "print ACL Window Station");
}

/************************************************************/

int 
main(int argc, char* argv[])
{
	WIN_NAMEIDATA wPath;
	char *prog = *argv++;
	char *token = "";
	char *cmd = NULL;
	char *arg = NULL;

	while (arg = *argv++){
		if (!strcmp(arg, "-C")){
//			_ShowCaps = TRUE;

		}else if (!strcmp(arg, "-o")){
			argv++;

		}else if (arg[0] == '-'){
			cmd = arg;

		}else{
			token = arg;

		}
	}
	if (!cmd){
		print_usage();

	}else if (!strcmp(cmd, "-d")){
		print_acl_desktop();

	}else if (!strcmp(cmd, "-s")){
		print_acl_station();

	}else if (!strcmp(cmd, "-p")){
		print_acl_process();

	}else if (!strcmp(cmd, "-P")){
		print_process_token(atoi(token));

	}else if (!strcmp(cmd, "-T")){
		print_thread_token(atoi(token));

	}else if (!strcmp(cmd, "-f")){
		print_acl_file(path_win(&wPath, token, O_NOFOLLOW)->Resolved);

	}else if (!strcmp(cmd, "-o")){
		print_acl_object(token);

	}else{
		printf("%s: No such option.\n", cmd);

	}
}
