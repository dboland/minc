#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(mlockall)
	push $SYS_mlockall
	jmp _syscall
	nop
