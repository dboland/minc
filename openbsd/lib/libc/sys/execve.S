#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(execve)
	push $SYS_execve
	jmp _syscall
	nop
