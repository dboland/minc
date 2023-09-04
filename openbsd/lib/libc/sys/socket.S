#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(socket)
	push $SYS_socket
	jmp _syscall
	nop
