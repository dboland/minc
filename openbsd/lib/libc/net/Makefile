OBJECTS= send.o recv.o herror.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/net/%.c
	${XGCC} -c ${CFLAGS} -I${SRCDIR}/sys -o $@ $<

