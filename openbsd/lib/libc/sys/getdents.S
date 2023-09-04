#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getdents)
	push $SYS_getdents
	jmp _syscall
	nop
