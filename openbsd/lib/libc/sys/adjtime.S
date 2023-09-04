#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(adjtime)
	push $SYS_adjtime
	jmp _syscall
	nop
