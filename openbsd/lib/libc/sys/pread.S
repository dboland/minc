#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(pread)
	push $SYS_pread
	jmp _syscall
	nop
