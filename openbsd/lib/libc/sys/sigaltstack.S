#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(sigaltstack)
	push $SYS_sigaltstack
	jmp _syscall
	nop
