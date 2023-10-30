#include "SYS.h"

ENTRY(_setlogin)
	pop %ecx
	mov $SYS_setlogin,%eax
	call ___syscall
	push %ecx
	ret
