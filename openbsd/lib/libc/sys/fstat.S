#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(fstat)
	push $SYS_fstat
	jmp _syscall
	nop
