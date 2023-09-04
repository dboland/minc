#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(recvmsg)
	push $SYS_recvmsg
	jmp _syscall
	nop
