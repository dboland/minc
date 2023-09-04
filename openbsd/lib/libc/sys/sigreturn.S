#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(sigreturn)
	push $SYS_sigreturn
	jmp _syscall
	nop
