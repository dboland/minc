#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getitimer)
	push $SYS_getitimer
	jmp _syscall
	nop
