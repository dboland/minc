#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(setpriority)
	push $SYS_setpriority
	jmp _syscall
	nop
