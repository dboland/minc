#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(setreuid)
	push $SYS_setreuid
	jmp _syscall
	nop
