#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(fchdir)
	push $SYS_fchdir
	jmp _syscall
	nop
