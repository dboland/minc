#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getresuid)
	push $SYS_getresuid
	jmp _syscall
	nop
