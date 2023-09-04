#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(ptrace)
	push $SYS_ptrace
	jmp _syscall
	nop
