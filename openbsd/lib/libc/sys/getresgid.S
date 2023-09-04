#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(getresgid)
	push $SYS_getresgid
	jmp _syscall
	nop
