#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(mkdirat)
	push $SYS_mkdirat
	jmp _syscall
	nop
