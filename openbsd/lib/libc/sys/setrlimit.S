#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(setrlimit)
	push $SYS_setrlimit
	jmp _syscall
	nop
