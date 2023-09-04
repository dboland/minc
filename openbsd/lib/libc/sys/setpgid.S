#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(setpgid)
	push $SYS_setpgid
	jmp _syscall
	nop
