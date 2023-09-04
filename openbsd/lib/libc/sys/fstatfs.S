#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(fstatfs)
	push $SYS_fstatfs
	jmp _syscall
	nop
