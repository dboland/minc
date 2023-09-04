#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(setregid)
	push $SYS_setregid
	jmp _syscall
	nop
