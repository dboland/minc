OBJECTS= citrus_ctype.o citrus_none.o citrus_utf8.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/citrus/%.c
	${XGCC} -c ${CFLAGS} -o $@ $<

citrus_utf8.o: citrus_utf8.c
	${XGCC} -c ${CFLAGS} -I${SRCDIR}/lib/libc/citrus -o $@ $<
