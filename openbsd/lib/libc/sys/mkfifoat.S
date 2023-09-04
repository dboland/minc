#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(mkfifoat)
	push $SYS_mkfifoat
	jmp _syscall
	nop
