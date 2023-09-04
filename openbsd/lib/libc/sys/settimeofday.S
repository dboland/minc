#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(settimeofday)
	push $SYS_settimeofday
	jmp _syscall
	nop
