#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(fhopen)
	push $SYS_fhopen
	jmp _syscall
	nop
