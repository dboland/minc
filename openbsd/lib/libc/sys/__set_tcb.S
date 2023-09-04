#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(__set_tcb)
	push $SYS___set_tcb
	jmp _syscall
	nop
