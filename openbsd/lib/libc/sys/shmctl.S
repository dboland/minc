#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(shmctl)
	push $SYS_shmctl
	jmp _syscall
	nop
