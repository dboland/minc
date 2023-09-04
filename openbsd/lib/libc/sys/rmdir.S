#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(rmdir)
	push $SYS_rmdir
	jmp _syscall
	nop
