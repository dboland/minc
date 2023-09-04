#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(seteuid)
	push $SYS_seteuid
	jmp _syscall
	nop
