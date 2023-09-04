#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(open)
	push $SYS_open
	jmp _syscall
	nop
