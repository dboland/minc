#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(kevent)
	push $SYS_kevent
	jmp _syscall
	nop
