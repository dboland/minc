OBJECTS= send.o recv.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/net/%.c
	gcc -c ${CFLAGS} -o $@ $<

