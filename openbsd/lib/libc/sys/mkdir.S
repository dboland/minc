#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(mkdir)
	push $SYS_mkdir
	jmp _syscall
	nop
