#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(recvfrom)
	push $SYS_recvfrom
	jmp _syscall
	nop
