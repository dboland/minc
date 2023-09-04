#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getsid)
	push $SYS_getsid
	jmp _syscall
	nop
