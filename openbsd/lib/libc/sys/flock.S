#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(flock)
	push $SYS_flock
	jmp _syscall
	nop
