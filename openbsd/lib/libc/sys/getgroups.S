#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getgroups)
	push $SYS_getgroups
	jmp _syscall
	nop
