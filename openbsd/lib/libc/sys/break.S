#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(break)
	push $SYS_break
	jmp _syscall
	nop
