#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(revoke)
	push $SYS_revoke
	jmp _syscall
	nop
