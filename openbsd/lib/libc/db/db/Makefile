OBJECTS=db.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/db/db/%.c
	gcc -c ${CFLAGS} -o $@ $<

