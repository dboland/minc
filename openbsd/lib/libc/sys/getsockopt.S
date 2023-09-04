#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getsockopt)
	push $SYS_getsockopt
	jmp _syscall
	nop
