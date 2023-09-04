#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(unlinkat)
	push $SYS_unlinkat
	jmp _syscall
	nop
