#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(munlockall)
	push $SYS_munlockall
	jmp _syscall
	nop
