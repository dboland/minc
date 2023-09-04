#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(clock_settime)
	push $SYS_clock_settime
	jmp _syscall
	nop
