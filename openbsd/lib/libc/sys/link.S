#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(link)
	push $SYS_link
	jmp _syscall
	nop
