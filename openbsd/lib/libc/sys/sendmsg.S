#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(sendmsg)
	push $SYS_sendmsg
	jmp _syscall
	nop
