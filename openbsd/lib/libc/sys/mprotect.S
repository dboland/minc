#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(mprotect)
	push $SYS_mprotect
	jmp _syscall
	nop
