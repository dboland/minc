OBJECTS=cp.o utils.o

%.o: ${SRCDIR}/bin/cp/%.c
	gcc -c ${CFLAGS} -o $@ $<

../cp.exe: ${OBJECTS}
	gcc -s -static ${CFLAGS} -o $@ $^ -lutil
