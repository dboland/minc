#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(fchown)
	push $SYS_fchown
	jmp _syscall
	nop
