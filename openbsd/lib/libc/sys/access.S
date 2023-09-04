#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(access)
	push $SYS_access
	jmp _syscall
	nop
