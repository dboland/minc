#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(nfssvc)
	push $SYS_nfssvc
	jmp _syscall
	nop
