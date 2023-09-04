#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(mount)
	push $SYS_mount
	jmp _syscall
	nop
