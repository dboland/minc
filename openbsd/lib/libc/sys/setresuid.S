#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(setresuid)
	push $SYS_setresuid
	jmp _syscall
	nop
