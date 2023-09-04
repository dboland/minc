#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(setgid)
	push $SYS_setgid
	jmp _syscall
	nop
