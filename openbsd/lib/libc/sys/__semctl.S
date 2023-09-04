#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(__semctl)
	push $SYS___semctl
	jmp _syscall
	nop
