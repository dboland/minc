#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(nanosleep)
	push $SYS_nanosleep
	jmp _syscall
	nop
