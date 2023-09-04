#include <machine/asm.h>
#include <sys/syscall.h>

ENTRY(profil)
	push $SYS_profil
	jmp _syscall
	nop
