#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(geteuid)
	push $SYS_geteuid
	jmp _syscall
	nop
