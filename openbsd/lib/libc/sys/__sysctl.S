#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(__sysctl)
	push $SYS___sysctl
	jmp _syscall
	nop
