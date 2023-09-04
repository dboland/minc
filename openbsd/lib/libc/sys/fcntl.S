#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(fcntl)
	push $SYS_fcntl
	jmp _syscall
	nop
