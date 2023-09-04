#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(lchown)
	push $SYS_lchown
	jmp _syscall
	nop
