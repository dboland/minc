#include "SYS.h"

ENTRY(syscall)
	pop %ecx
	pop %eax
	call ___kernel
	push %ecx
	ret
