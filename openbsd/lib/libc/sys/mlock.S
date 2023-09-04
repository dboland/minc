#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(mlock)
	push $SYS_mlock
	jmp _syscall
	nop
