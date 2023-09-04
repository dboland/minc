#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(setegid)
	push $SYS_setegid
	jmp _syscall
	nop
