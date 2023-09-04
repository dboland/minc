#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(utrace)
	push $SYS_utrace
	jmp _syscall
	nop
