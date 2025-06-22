OBJECTS=main.o print.o cmp.o util.o ls.o

%.o: ${SRCDIR}/bin/ls/%.c
	gcc -c ${CFLAGS} -o $@ $<

../ls.exe: ${OBJECTS}
	gcc -s -static ${CFLAGS} -o $@ $^ -lutil
