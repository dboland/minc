#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(msgsnd)
	push $SYS_msgsnd
	jmp _syscall
	nop
