#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(__thrwakeup)
	push $SYS___thrwakeup
	jmp _syscall
	nop
