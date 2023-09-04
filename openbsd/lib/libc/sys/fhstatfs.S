#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(fhstatfs)
	push $SYS_fhstatfs
	jmp _syscall
	nop
