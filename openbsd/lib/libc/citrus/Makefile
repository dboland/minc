OBJECTS= citrus_ctype.o citrus_none.o
OBJECTS+= citrus_utf8.c.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/citrus/%.c
	gcc -c ${CFLAGS} -o $@ $<

%.c.o: %.c
	gcc -c ${CFLAGS} -I${SRCDIR}/lib/libc/citrus -o $@ $<
