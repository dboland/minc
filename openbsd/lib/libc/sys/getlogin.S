#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getlogin)
	push $SYS_getlogin
	jmp _syscall
	nop
