OBJECTS=dlfcn_stubs.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/dlfcn/%.c
	gcc -c ${CFLAGS} -o $@ $<

