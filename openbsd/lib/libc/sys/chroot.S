#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(chroot)
	push $SYS_chroot
	jmp _syscall
	nop
