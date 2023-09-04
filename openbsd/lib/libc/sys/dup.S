#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(dup)
	push $SYS_dup
	jmp _syscall
	nop
