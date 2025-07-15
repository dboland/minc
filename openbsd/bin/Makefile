include ../../Makefile.inc

PROGS= chmod ln mkdir mv rm rmdir test chown chgrp ls cp
OBJECTS= ${PROGS} cat
CFLAGS+= -DNOPIE -I${SRCDIR}/sys
BINSRC= ${SRCDIR}/bin

all: ${BINSRC} ${OBJECTS}
	@${MAKE} -C ksh

${BINSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'bin' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

${BINDIR}: ${PROGS}
	@/bin/mkdir -p ${BINDIR}
	/bin/cp -vu ${PROGS} ${BINDIR}/

chmod: ${BINSRC}/chmod/chmod.c
	gcc -s -static ${CFLAGS} -o $@ $<

chown: chmod.exe
	/bin/cp -vu $< $@.exe

chgrp: chmod.exe
	/bin/cp -vu $< $@.exe

ln: ${BINSRC}/ln/ln.c
	gcc -s -static ${CFLAGS} -o $@ $<

mkdir: ${BINSRC}/mkdir/mkdir.c
	gcc -s -static ${CFLAGS} -o $@ $<

mv: ${BINSRC}/mv/mv.c
	gcc -s -static ${CFLAGS} -o $@ $<

rm: ${BINSRC}/rm/rm.c
	gcc -s -static ${CFLAGS} -o $@ $<

rmdir: ${BINSRC}/rmdir/rmdir.c
	gcc -s -static ${CFLAGS} -o $@ $<

test: ${BINSRC}/test/test.c
	gcc -s -static ${CFLAGS} -o $@ $<

cat: ${BINSRC}/cat/cat.c
	gcc -s -static ${CFLAGS} -o $@ $<

ls:
	@gcc -c ${CFLAGS} -o ls-main.o ls-main.c
	@gcc -c ${CFLAGS} -o print.o ${BINSRC}/ls/print.c
	@gcc -c ${CFLAGS} -o cmp.o ${BINSRC}/ls/cmp.c
	@gcc -c ${CFLAGS} -o util.o ${BINSRC}/ls/util.c
	@gcc -c ${CFLAGS} -o ls.o ${BINSRC}/ls/ls.c
	gcc -s -static ${CFLAGS} -o $@ ls-main.o print.o cmp.o util.o ls.o -lutil

cp:
	@gcc -c ${CFLAGS} -o utils.o ${BINSRC}/cp/utils.c
	@gcc -c ${CFLAGS} -o cp.o ${BINSRC}/cp/cp.c
	gcc -s -static ${CFLAGS} -o $@ utils.o cp.o

install-local: ${BINDIR}

install: ${DESTDIR} ${OBJECTS}
	@mkdir -p ${DESTDIR}/bin
	${INSTALL} ${OBJECTS} ${DESTDIR}/bin/
	@${MAKE} -C ksh install

clean:
	/bin/rm -f *.exe *.o
	@${MAKE} -C ksh clean
