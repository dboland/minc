#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(truncate)
	push $SYS_truncate
	jmp _syscall
	nop
