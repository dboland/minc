OBJECTS=dmisc.o dtoa.o gdtoa.o gmisc.o hd_init.o \
	hexnan.o locks.o misc.o smisc.o \
	strtold.o sum.o ulp.o

OBJECTS+= hdtoa.o ldtoa.o strtod.o strtord.o strtodg.o strtof.o gethex.o 

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}
	@/bin/echo "done."

%.o: ${SRCDIR}/lib/libc/gdtoa/%.c
	@/bin/echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -I${SRCDIR}/lib/libc/arch/${ARCH}/gdtoa -o $@ $<

clean:
	rm -f *.o *.dll *.a
