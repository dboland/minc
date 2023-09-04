#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(futimens)
	push $SYS_futimens
	jmp _syscall
	nop
