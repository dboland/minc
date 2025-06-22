#OBJECTS=syscall.S.o fork.S.o vfork.S.o cerror.S.o
OBJECTS=syscall.S.o fork.S.o vfork.S.o
OBJECTS+=_exit.S.o _getlogin.S.o _setlogin.S.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/arch/${ARCH}/sys/%.S
	gcc -c ${CFLAGS} -o $@ $<

%.S.o: %.S
	gcc -c ${CFLAGS} -I.. -o $@ $<
