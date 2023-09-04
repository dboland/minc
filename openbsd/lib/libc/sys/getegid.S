#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getegid)
	push $SYS_getegid
	jmp _syscall
	nop
