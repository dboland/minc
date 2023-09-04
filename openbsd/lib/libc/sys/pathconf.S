#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(pathconf)
	push $SYS_pathconf
	jmp _syscall
	nop
