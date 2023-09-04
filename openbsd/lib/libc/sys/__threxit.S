#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(__threxit)
	push $SYS___threxit
	jmp _syscall
	nop
