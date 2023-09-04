#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(readlink)
	push $SYS_readlink
	jmp _syscall
	nop
