#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(fchmod)
	push $SYS_fchmod
	jmp _syscall
	nop
