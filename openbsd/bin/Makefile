include ../../Makefile.inc

OBJECTS= chmod ln mkdir mv rm rmdir test chown chgrp ls cp
CFLAGS+= -DNOPIE -I${SRCDIR}/sys
LBINDIR= ${PREFIX}/local/bin
BINPATH= ${SRCDIR}/bin

all: ${BINPATH} ${OBJECTS} cat
	@${MAKE} -C ksh

${BINPATH}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'bin' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

${LBINDIR}: ${OBJECTS}
	@/bin/mkdir -p ${LBINDIR}
	/bin/cp -vu ${OBJECTS} ${LBINDIR}/

chmod: ${SRCDIR}/bin/chmod/chmod.c
	gcc -s -static ${CFLAGS} -o $@ $<

chown: chmod.exe
	/bin/cp -vu $< $@.exe

chgrp: chmod.exe
	/bin/cp -vu $< $@.exe

ln: ${SRCDIR}/bin/ln/ln.c
	gcc -s -static ${CFLAGS} -o $@ $<

mkdir: ${SRCDIR}/bin/mkdir/mkdir.c
	gcc -s -static ${CFLAGS} -o $@ $<

mv: ${SRCDIR}/bin/mv/mv.c
	gcc -s -static ${CFLAGS} -o $@ $<

rm: ${SRCDIR}/bin/rm/rm.c
	gcc -s -static ${CFLAGS} -o $@ $<

rmdir: ${SRCDIR}/bin/rmdir/rmdir.c
	gcc -s -static ${CFLAGS} -o $@ $<

test: ${SRCDIR}/bin/test/test.c
	gcc -s -static ${CFLAGS} -o $@ $<

cat: ${SRCDIR}/bin/cat/cat.c
	gcc -s -static ${CFLAGS} -o $@ $<

ls:
	@gcc -c ${CFLAGS} -o main.o ${SRCDIR}/bin/ls/main.c
	@gcc -c ${CFLAGS} -o print.o ${SRCDIR}/bin/ls/print.c
	@gcc -c ${CFLAGS} -o cmp.o ${SRCDIR}/bin/ls/cmp.c
	@gcc -c ${CFLAGS} -o util.o ${SRCDIR}/bin/ls/util.c
	@gcc -c ${CFLAGS} -o ls.o ${SRCDIR}/bin/ls/ls.c
	gcc -s -static ${CFLAGS} -o $@ main.o print.o cmp.o util.o ls.o -lutil

cp:
	@gcc -c ${CFLAGS} -o utils.o ${SRCDIR}/bin/cp/utils.c
	@gcc -c ${CFLAGS} -o cp.o ${SRCDIR}/bin/cp/cp.c
	gcc -s -static ${CFLAGS} -o $@ utils.o cp.o

install-local: ${LBINDIR}

install:
	@mkdir -p ${DESTDIR}${BINDIR}
	${INSTALL} ${OBJECTS} cat ${DESTDIR}${BINDIR}/
	@${MAKE} -C ksh install

clean:
	/bin/rm -f *.exe *.o
	@${MAKE} -C ksh clean
