#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(openat)
	push $SYS_openat
	jmp _syscall
	nop
