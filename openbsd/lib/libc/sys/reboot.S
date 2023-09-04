#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(reboot)
	push $SYS_reboot
	jmp _syscall
	nop
