#include "SYS.h"

ENTRY(_exit)
	push $SYS_exit
	jmp _syscall
