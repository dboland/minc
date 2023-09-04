#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(ioctl)
	push $SYS_ioctl
	jmp _syscall
	nop
