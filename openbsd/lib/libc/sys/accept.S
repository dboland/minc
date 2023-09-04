#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(accept)
	push $SYS_accept
	jmp _syscall
	nop
