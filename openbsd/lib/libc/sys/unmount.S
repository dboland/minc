#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(unmount)
	push $SYS_unmount
	jmp _syscall
	nop
