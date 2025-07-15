include ../../Makefile.inc

PROGS= arch uname touch
OBJECTS= ${PROGS} grep du env
CFLAGS+= -I${SRCDIR}/sys
UBINSRC= ${SRCDIR}/usr.bin
UBINDIR= /usr/bin

all: ${UBINSRC} ${OBJECTS} install.exe

${UBINSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'usr.bin' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

${BINDIR}: ${PROGS} install.exe
	@/bin/mkdir -p ${BINDIR}
	/bin/cp -vu ${PROGS} install.exe install.exe.manifest ${BINDIR}/

uname: ${UBINSRC}/uname/uname.c
	gcc -s -static ${CFLAGS} -DNOPIE -o $@ $<

arch: ${UBINSRC}/arch/arch.c
	gcc -s -static ${CFLAGS} -DNOPIE -o $@ $<

touch: ${UBINSRC}/touch/touch.c
	gcc -s -static ${CFLAGS} -DNOPIE -o $@ $<

# In Windows, "install" seems to be a reserved word. When 
# executed with such name, Vista checks if user is elevated, 
# emits ERROR_ELEVATION_REQUIRED if not.
# see: https://irq5.io/2011/06/26/gnu-patch-and-windows-uac/

install.exe: ${UBINSRC}/xinstall/xinstall.c
	gcc -s -static ${CFLAGS} -DNOPIE -o $@ $<

grep: grep.c
	@gcc -c ${CFLAGS} -o binary.o ${UBINSRC}/grep/binary.c
	@gcc -c ${CFLAGS} -o file.o ${UBINSRC}/grep/file.c
	@gcc -c ${CFLAGS} -o mmfile.o ${UBINSRC}/grep/mmfile.c
	@gcc -c ${CFLAGS} -o queue.o ${UBINSRC}/grep/queue.c
	@gcc -c ${CFLAGS} -o util.o ${UBINSRC}/grep/util.c
	gcc -s ${CFLAGS} -I${UBINSRC}/grep -o $@ $< binary.o file.o mmfile.o queue.o util.o -lz

du: ${UBINSRC}/du/du.c
	gcc -s ${CFLAGS} -o $@ $< -lutil

env: env.c
	gcc -s ${CFLAGS} -o $@ $<

install-local: ${BINDIR}

install: ${DESTDIR} ${OBJECTS} install.exe
	@mkdir -p ${DESTDIR}${UBINDIR}
	${INSTALL} ${OBJECTS} install install.manifest ${DESTDIR}${UBINDIR}/

clean:
	/bin/rm -f *.exe *.o
