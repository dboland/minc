OBJECTS = regcomp.o regerror.o regexec.o regfree.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/regex/%.c
	${XGCC} -c ${CFLAGS} -o $@ $<

clean:
	rm -f *.o
