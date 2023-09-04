#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(writev)
	push $SYS_writev
	jmp _syscall
	nop
