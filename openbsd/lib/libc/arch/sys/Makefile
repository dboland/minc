OBJECTS=cerror.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/arch/${ARCH}/sys/%.S
	gcc -c ${CFLAGS} -I${SRCDIR}/arch/${ARCH} -o $@ $<

