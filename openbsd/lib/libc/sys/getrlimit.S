#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getrlimit)
	push $SYS_getrlimit
	jmp _syscall
	nop
