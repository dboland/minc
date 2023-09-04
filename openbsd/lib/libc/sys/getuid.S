#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getuid)
	push $SYS_getuid
	jmp _syscall
	nop
