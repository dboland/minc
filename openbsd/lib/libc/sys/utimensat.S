#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(utimensat)
	push $SYS_utimensat
	jmp _syscall
	nop
