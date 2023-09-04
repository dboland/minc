#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(issetugid)
	push $SYS_issetugid
	jmp _syscall
	nop
