#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(setitimer)
	push $SYS_setitimer
	jmp _syscall
	nop
