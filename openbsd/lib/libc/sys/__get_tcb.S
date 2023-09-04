#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(__get_tcb)
	push $SYS___get_tcb
	jmp _syscall
	nop
