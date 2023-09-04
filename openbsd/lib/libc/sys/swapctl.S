#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(swapctl)
	push $SYS_swapctl
	jmp _syscall
	nop
