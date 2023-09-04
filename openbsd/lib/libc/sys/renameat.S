#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(renameat)
	push $SYS_renameat
	jmp _syscall
	nop
