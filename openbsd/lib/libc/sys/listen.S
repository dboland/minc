#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(listen)
	push $SYS_listen
	jmp _syscall
	nop
