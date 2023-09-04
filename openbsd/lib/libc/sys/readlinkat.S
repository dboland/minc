#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(readlinkat)
	push $SYS_readlinkat
	jmp _syscall
	nop
