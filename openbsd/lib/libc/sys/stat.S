#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(stat)
	push $SYS_stat
	jmp _syscall
	nop
