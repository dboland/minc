#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(symlink)
	push $SYS_symlink
	jmp _syscall
	nop
