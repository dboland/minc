#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(faccessat)
	push $SYS_faccessat
	jmp _syscall
	nop
