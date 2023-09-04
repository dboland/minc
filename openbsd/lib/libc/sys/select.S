#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(select)
	push $SYS_select
	jmp _syscall
	nop
