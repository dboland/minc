#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(connect)
	push $SYS_connect
	jmp _syscall
	nop
