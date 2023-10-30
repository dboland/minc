#include "SYS.h"

ENTRY(_getlogin)
	pop %ecx
	mov $SYS_getlogin,%eax
	call ___syscall
	push %ecx
	ret
