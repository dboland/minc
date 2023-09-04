#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(sigprocmask)
	push $SYS_sigprocmask
	jmp _syscall
	nop
