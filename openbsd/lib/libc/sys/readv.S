#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(readv)
	push $SYS_readv
	jmp _syscall
	nop
