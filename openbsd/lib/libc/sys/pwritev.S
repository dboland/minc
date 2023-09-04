#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(pwritev)
	push $SYS_pwritev
	jmp _syscall
	nop
