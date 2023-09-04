#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(setuid)
	push $SYS_setuid
	jmp _syscall
	nop
