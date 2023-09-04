#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(_getlogin)
	push $SYS_getlogin
	jmp _syscall
	nop
