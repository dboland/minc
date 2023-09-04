#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(setrtable)
	push $SYS_setrtable
	jmp _syscall
	nop
