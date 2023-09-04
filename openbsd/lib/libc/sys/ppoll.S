#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(ppoll)
	push $SYS_ppoll
	jmp _syscall
	nop
