OBJECTS=mpool.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/db/mpool/%.c
	${XGCC} -c ${CFLAGS} -o $@ $<

