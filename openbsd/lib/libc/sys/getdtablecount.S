#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getdtablecount)
	push $SYS_getdtablecount
	jmp _syscall
	nop
