OBJECTS=mpool.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/db/mpool/%.c
	gcc -c ${CFLAGS} -o $@ $<

