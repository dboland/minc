#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(pwrite)
	push $SYS_pwrite
	jmp _syscall
	nop
