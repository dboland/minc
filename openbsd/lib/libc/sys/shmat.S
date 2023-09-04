#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(shmat)
	push $SYS_shmat
	jmp _syscall
	nop
