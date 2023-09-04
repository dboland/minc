#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(msgctl)
	push $SYS_msgctl
	jmp _syscall
	nop
