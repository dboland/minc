#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(fstatat)
	push $SYS_fstatat
	jmp _syscall
	nop
