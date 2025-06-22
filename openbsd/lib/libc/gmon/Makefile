OBJECTS=gmon.o
OBJECTS+=mcount.c.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/gmon/%.c
	gcc -c ${CFLAGS} -I${SRCDIR}/sys -o $@ $<

%.c.o: %.c
	gcc -c ${CFLAGS} -I${SRCDIR}/sys -o $@ $<
