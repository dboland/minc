#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(shmdt)
	push $SYS_shmdt
	jmp _syscall
	nop
