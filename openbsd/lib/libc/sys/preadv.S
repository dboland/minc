#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(preadv)
	push $SYS_preadv
	jmp _syscall
	nop
