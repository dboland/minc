#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getfsstat)
	push $SYS_getfsstat
	jmp _syscall
	nop
