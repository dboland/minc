#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(mmap)
	push $SYS_mmap
	jmp _syscall
	nop
