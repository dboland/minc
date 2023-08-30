OBJECTS=dmisc.o dtoa.o gdtoa.o gmisc.o hd_init.o \
	hexnan.o locks.o misc.o smisc.o \
	strtold.o sum.o ulp.o

OBJECTS+= hdtoa.o ldtoa.o strtod.o strtord.o strtodg.o strtof.o gethex.o 

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/gdtoa/%.c
	gcc -c ${CFLAGS} -I${SRCDIR}/arch/${ARCH}/gdtoa -o $@ $<

testlib: ${OBJECTS}
	gcc -shared -e "" -Wl,--disable-auto-import -o test.dll ${OBJECTS} ../gen/libgen.a

libgdtoa.a: ${OBJECTS}
	ar cru libgdtoa.a ${OBJECTS}

clean:
	rm -f *.o *.dll *.a
