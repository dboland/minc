#include "SYS.h"

ENTRY(_getlogin)
	push $SYS_getlogin
	jmp _syscall
