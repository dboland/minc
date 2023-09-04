#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(pselect)
	push $SYS_pselect
	jmp _syscall
	nop
