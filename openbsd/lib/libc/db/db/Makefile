OBJECTS=db.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/db/db/%.c
	gcc -c ${CFLAGS} -o $@ $<

