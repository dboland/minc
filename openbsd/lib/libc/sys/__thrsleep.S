#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(__thrsleep)
	push $SYS___thrsleep
	jmp _syscall
	nop
