#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(msync)
	push $SYS_msync
	jmp _syscall
	nop
