#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(sysarch)
	push $SYS_sysarch
	jmp _syscall
	nop
