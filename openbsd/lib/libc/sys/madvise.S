#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(madvise)
	push $SYS_madvise
	jmp _syscall
	nop
