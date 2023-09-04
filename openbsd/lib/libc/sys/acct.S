#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(acct)
	push $SYS_acct
	jmp _syscall
	nop
