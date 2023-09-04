#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(pipe)
	push $SYS_pipe
	jmp _syscall
	nop
