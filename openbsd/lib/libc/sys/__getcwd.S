#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(__getcwd)
	push $SYS___getcwd
	jmp _syscall
	nop
