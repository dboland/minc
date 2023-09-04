#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(closefrom)
	push $SYS_closefrom
	jmp _syscall
	nop
