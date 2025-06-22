OBJECTS=_setjmp.o fabs.o infinity.c.o ldexp.c.o \
	modf.o nan.c.o setjmp.o sigsetjmp.o
OBJECTS+=fpclassifyl.c.o isfinitel.c.o isinfl.c.o isnanl.c.o isnormall.c.o signbitl.c.o
OBJECTS+=flt_rounds.o fpgetmask.o fpgetround.o fpgetsticky.o fpsetmask.o \
	fpsetround.o fpsetsticky.o
OBJECTS+=divsi3.o fixdfsi.o fixunsdfsi.o udivsi3.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.c.o: ${SRCDIR}/lib/libc/arch/i386/gen/%.c
	gcc -c ${CFLAGS} -o $@ $<

%.o: ${SRCDIR}/lib/libc/arch/i386/gen/%.S
	gcc -c ${CFLAGS} -o $@ $<
