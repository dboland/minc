#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(linkat)
	push $SYS_linkat
	jmp _syscall
	nop
