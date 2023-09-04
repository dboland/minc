#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(ktrace)
	push $SYS_ktrace
	jmp _syscall
	nop
