#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(__tfork)
	push $SYS___tfork
	jmp _syscall
	nop
