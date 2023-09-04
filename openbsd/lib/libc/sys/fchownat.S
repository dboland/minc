#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(fchownat)
	push $SYS_fchownat
	jmp _syscall
	nop
