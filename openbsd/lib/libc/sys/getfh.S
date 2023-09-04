#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getfh)
	push $SYS_getfh
	jmp _syscall
	nop
