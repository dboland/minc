#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(shutdown)
	push $SYS_shutdown
	jmp _syscall
	nop
