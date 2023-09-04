#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(futimes)
	push $SYS_futimes
	jmp _syscall
	nop
