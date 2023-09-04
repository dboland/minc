#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(quotactl)
	push $SYS_quotactl
	jmp _syscall
	nop
