#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(chmod)
	push $SYS_chmod
	jmp _syscall
	nop
