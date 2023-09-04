#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(mincore)
	push $SYS_mincore
	jmp _syscall
	nop
