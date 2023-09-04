#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(minherit)
	push $SYS_minherit
	jmp _syscall
	nop
