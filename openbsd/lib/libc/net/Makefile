OBJECTS= send.o recv.o herror.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/net/%.c
	gcc -c ${CFLAGS} -o $@ $<

