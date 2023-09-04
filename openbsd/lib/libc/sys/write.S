#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(write)
	push $SYS_write
	jmp _syscall
	nop
