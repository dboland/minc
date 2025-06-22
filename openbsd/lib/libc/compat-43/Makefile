OBJECTS= creat.o getdtablesize.o getwd.o \
	setpgrp.o sigcompat.o killpg.o gethostid.o sethostid.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/compat-43/%.c
	gcc -c ${CFLAGS} -I${SRCDIR}/sys -o $@ $<

testlib: ${OBJECTS}
	gcc -shared -e "" -Wl,--disable-auto-import -o testlib.dll ${OBJECTS}

libcompat-43.a: ${OBJECTS}
	ar cru libcompat-43.a ${OBJECTS}

clean:
	rm -f *.o *.dll *.a
