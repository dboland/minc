#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(sigpending)
	push $SYS_sigpending
	jmp _syscall
	nop
