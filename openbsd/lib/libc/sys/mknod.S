#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(mknod)
	push $SYS_mknod
	jmp _syscall
	nop
