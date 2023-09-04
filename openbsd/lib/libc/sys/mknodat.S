#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(mknodat)
	push $SYS_mknodat
	jmp _syscall
	nop
