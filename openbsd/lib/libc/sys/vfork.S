#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(vfork)
	push $SYS_vfork
	jmp _syscall
	nop
