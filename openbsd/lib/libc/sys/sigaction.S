#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(sigaction)
	push $SYS_sigaction
	jmp _syscall
	nop
