#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getpgrp)
	push $SYS_getpgrp
	jmp _syscall
	nop
