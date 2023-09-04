#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(setgroups)
	push $SYS_setgroups
	jmp _syscall
	nop
