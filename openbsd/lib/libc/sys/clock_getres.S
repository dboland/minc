#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(clock_getres)
	push $SYS_clock_getres
	jmp _syscall
	nop
