#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(statfs)
	push $SYS_statfs
	jmp _syscall
	nop
