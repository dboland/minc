#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(semop)
	push $SYS_semop
	jmp _syscall
	nop
