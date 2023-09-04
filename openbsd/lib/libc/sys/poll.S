#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(poll)
	push $SYS_poll
	jmp _syscall
	nop
