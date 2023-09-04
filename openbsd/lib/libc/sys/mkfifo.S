#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(mkfifo)
	push $SYS_mkfifo
	jmp _syscall
	nop
