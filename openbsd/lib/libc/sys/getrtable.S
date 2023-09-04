#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getrtable)
	push $SYS_getrtable
	jmp _syscall
	nop
