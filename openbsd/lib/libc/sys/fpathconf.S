#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(fpathconf)
	push $SYS_fpathconf
	jmp _syscall
	nop
