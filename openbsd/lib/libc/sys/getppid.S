#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getppid)
	push $SYS_getppid
	jmp _syscall
	nop
