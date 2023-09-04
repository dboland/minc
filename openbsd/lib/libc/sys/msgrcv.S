#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(msgrcv)
	push $SYS_msgrcv
	jmp _syscall
	nop
