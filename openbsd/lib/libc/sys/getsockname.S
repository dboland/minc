#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getsockname)
	push $SYS_getsockname
	jmp _syscall
	nop
