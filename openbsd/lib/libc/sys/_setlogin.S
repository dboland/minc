#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(_setlogin)
	push $SYS_setlogin
	jmp _syscall
	nop
