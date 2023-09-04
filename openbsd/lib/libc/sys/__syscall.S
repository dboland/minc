#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(__syscall)
	push $SYS___syscall
	jmp _syscall
	nop
