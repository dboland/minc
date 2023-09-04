#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getthrid)
	push $SYS_getthrid
	jmp _syscall
	nop
