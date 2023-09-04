#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(sigsuspend)
	push $SYS_sigsuspend
	jmp _syscall
	nop
