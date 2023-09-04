#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(read)
	push $SYS_read
	jmp _syscall
	nop
