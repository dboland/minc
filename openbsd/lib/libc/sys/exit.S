#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(exit)
	push $SYS_exit
	jmp _syscall
	nop
