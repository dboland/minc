#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(unlink)
	push $SYS_unlink
	jmp _syscall
	nop
