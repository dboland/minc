#OBJECTS=syscall.S.o fork.S.o vfork.S.o cerror.S.o
OBJECTS=syscall.S.o fork.S.o vfork.S.o
OBJECTS+=_exit.S.o _getlogin.S.o _setlogin.S.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}
	@/bin/echo "done."

%.o: ${SRCDIR}/lib/libc/arch/${ARCH}/sys/%.S
	@/bin/echo -n "$@ "
	@gcc -c ${CFLAGS} -o $@ $<

%.S.o: %.S
	@/bin/echo -n "$@ "
	@gcc -c ${CFLAGS} -I.. -o $@ $<
