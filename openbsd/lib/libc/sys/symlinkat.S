#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(symlinkat)
	push $SYS_symlinkat
	jmp _syscall
	nop
