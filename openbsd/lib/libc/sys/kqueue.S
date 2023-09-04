#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(kqueue)
	push $SYS_kqueue
	jmp _syscall
	nop
