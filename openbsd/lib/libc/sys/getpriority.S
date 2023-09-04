#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getpriority)
	push $SYS_getpriority
	jmp _syscall
	nop
