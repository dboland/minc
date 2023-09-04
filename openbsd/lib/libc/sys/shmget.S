#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(shmget)
	push $SYS_shmget
	jmp _syscall
	nop
