#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(MAXSYSCALL)
	push $SYS_MAXSYSCALL
	jmp _syscall
	nop
