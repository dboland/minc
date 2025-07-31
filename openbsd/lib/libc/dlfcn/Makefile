OBJECTS=dlfcn_stubs.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/dlfcn/%.c
	${XGCC} -c ${CFLAGS} -o $@ $<

