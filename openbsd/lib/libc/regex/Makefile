CPPFLAGS= -Wl,--disable-auto-import

OBJECTS = regcomp.o regerror.o regexec.o regfree.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/regex/%.c
	gcc -c ${CFLAGS} -o $@ $<

testlib: ${OBJECTS}
	gcc -shared ${CPPFLAGS} -e "" -Wl,--disable-auto-import -o testlib.dll ${OBJECTS} \
		../libc.o ../stdlib/libstd.a

libregex.a: ${OBJECTS}
	ar cru libregex.a ${OBJECTS}

clean:
	rm -f *.o *.dll *.a
