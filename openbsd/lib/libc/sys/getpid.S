#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getpid)
	push $SYS_getpid
	jmp _syscall
	nop
