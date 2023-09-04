#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(kill)
	push $SYS_kill
	jmp _syscall
	nop
