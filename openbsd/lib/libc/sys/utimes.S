#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(utimes)
	push $SYS_utimes
	jmp _syscall
	nop
