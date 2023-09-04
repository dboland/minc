#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getgid)
	push $SYS_getgid
	jmp _syscall
	nop
