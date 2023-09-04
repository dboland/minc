#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getrusage)
	push $SYS_getrusage
	jmp _syscall
	nop
