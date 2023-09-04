#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(umask)
	push $SYS_umask
	jmp _syscall
	nop
