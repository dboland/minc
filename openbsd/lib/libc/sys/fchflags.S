#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(fchflags)
	push $SYS_fchflags
	jmp _syscall
	nop
