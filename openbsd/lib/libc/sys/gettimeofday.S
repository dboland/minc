#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(gettimeofday)
	push $SYS_gettimeofday
	jmp _syscall
	nop
