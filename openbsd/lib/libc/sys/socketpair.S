#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(socketpair)
	push $SYS_socketpair
	jmp _syscall
	nop
