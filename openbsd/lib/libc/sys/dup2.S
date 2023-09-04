#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(dup2)
	push $SYS_dup2
	jmp _syscall
	nop
