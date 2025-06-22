OBJECTS=catclose.o catgets.o catopen.o
OBJECTS+=_catclose.o _catgets.o _catopen.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/nls/%.c
	gcc -c ${CFLAGS} -o $@ $<

