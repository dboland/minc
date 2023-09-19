#include "SYS.h"

ENTRY(_setlogin)
	push $SYS_setlogin
	jmp _syscall
