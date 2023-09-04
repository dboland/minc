#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(chown)
	push $SYS_chown
	jmp _syscall
	nop
