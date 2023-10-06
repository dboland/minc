#include "SYS.h"

ENTRY(vfork)
	push %fs:0x2c
	call ___tls_copy
	add $4,%esp
	push $SYS_vfork
	jmp _syscall
	nop
