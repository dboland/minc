#include "SYS.h"

ENTRY(vfork)
	push %fs:0x2c
	call ___tls_copy
	add $4,%esp
	pop %ecx
	mov $SYS_vfork,%eax
	call ___syscall
	push %ecx
	ret
