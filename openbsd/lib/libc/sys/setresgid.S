#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(setresgid)
	push $SYS_setresgid
	jmp _syscall
	nop
