#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(rename)
	push $SYS_rename
	jmp _syscall
	nop
