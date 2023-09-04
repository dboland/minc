#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(munlock)
	push $SYS_munlock
	jmp _syscall
	nop
