#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(fhstat)
	push $SYS_fhstat
	jmp _syscall
	nop
