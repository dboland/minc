#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(chflags)
	push $SYS_chflags
	jmp _syscall
	nop
