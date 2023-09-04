#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(chdir)
	push $SYS_chdir
	jmp _syscall
	nop
