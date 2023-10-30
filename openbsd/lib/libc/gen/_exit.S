#include "SYS.h"

ENTRY(_exit)
	pop %ecx
	mov $SYS_exit,%eax
	call ___syscall
	push %ecx
	ret
