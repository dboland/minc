#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(close)
	push $SYS_close
	jmp _syscall
	nop
