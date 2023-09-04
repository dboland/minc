#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(setsid)
	push $SYS_setsid
	jmp _syscall
	nop
