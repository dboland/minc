#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(__thrsigdivert)
	push $SYS___thrsigdivert
	jmp _syscall
	nop
