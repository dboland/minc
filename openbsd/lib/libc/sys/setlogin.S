#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(setlogin)
	push $SYS_setlogin
	jmp _syscall
	nop
