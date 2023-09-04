#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(sched_yield)
	push $SYS_sched_yield
	jmp _syscall
	nop
