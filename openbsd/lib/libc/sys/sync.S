#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(sync)
	push $SYS_sync
	jmp _syscall
	nop
