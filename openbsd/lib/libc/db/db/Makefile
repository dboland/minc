OBJECTS=db.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/db/db/%.c
	${XGCC} -c ${CFLAGS} -o $@ $<

