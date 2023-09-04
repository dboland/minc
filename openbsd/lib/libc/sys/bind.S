#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(bind)
	push $SYS_bind
	jmp _syscall
	nop
