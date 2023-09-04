#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(wait4)
	push $SYS_wait4
	jmp _syscall
	nop
