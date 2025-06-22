OBJECTS = regcomp.o regerror.o regexec.o regfree.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/regex/%.c
	gcc -c ${CFLAGS} -o $@ $<

clean:
	rm -f *.o
