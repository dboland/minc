#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(lstat)
	push $SYS_lstat
	jmp _syscall
	nop
