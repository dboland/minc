#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(sendto)
	push $SYS_sendto
	jmp _syscall
	nop
