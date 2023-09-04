#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(munmap)
	push $SYS_munmap
	jmp _syscall
	nop
