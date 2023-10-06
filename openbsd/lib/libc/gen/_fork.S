#include "SYS.h"

ENTRY(fork)
	push %fs:0x2c
	call ___tls_copy
	add $4,%esp
	push $SYS_fork
	jmp _syscall
	nop
