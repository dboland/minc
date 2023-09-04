#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(fchmodat)
	push $SYS_fchmodat
	jmp _syscall
	nop
