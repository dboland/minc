#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(clock_gettime)
	push $SYS_clock_gettime
	jmp _syscall
	nop
