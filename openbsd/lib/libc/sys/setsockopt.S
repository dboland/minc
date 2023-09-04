#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(setsockopt)
	push $SYS_setsockopt
	jmp _syscall
	nop
