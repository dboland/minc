#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(ftruncate)
	push $SYS_ftruncate
	jmp _syscall
	nop
