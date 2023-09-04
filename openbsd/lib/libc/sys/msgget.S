#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(msgget)
	push $SYS_msgget
	jmp _syscall
	nop
