#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(adjfreq)
	push $SYS_adjfreq
	jmp _syscall
	nop
