#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(fsync)
	push $SYS_fsync
	jmp _syscall
	nop
