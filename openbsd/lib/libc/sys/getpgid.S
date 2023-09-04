#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getpgid)
	push $SYS_getpgid
	jmp _syscall
	nop
