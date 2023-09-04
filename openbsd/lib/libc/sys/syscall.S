#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(syscall)
	push $SYS_syscall
	jmp _syscall
	nop
