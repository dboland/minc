#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(fork)
	push $SYS_fork
	jmp _syscall
	nop
