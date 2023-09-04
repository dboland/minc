#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(mquery)
	push $SYS_mquery
	jmp _syscall
	nop
