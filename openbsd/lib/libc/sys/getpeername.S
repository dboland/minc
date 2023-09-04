#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getpeername)
	push $SYS_getpeername
	jmp _syscall
	nop
