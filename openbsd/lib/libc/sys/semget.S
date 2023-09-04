#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(semget)
	push $SYS_semget
	jmp _syscall
	nop
