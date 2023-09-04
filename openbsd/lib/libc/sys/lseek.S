#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(lseek)
	push $SYS_lseek
	jmp _syscall
	nop
