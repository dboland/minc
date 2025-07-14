include ../../Makefile.inc

PROGS= arch uname touch
OBJECTS= ${PROGS} grep du env
CFLAGS+= -I${SRCDIR}/sys
UBINSRC= ${SRCDIR}/usr.bin

all: ${UBINSRC} ${OBJECTS} install.exe

${UBINSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'usr.bin' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

${UBINDIR}: ${PROGS} install.exe
	@/bin/mkdir -p ${UBINDIR}
	/bin/cp -vu ${PROGS} install.exe install.exe.manifest ${UBINDIR}/

uname: ${SRCDIR}/usr.bin/uname/uname.c
	gcc -s -static ${CFLAGS} -DNOPIE -o $@ $<

arch: ${SRCDIR}/usr.bin/arch/arch.c
	gcc -s -static ${CFLAGS} -DNOPIE -o $@ $<

touch: ${SRCDIR}/usr.bin/touch/touch.c
	gcc -s -static ${CFLAGS} -DNOPIE -o $@ $<

# In Windows, "install" seems to be a reserved word. When 
# executed with such name, Vista checks if user is elevated, 
# emits ERROR_ELEVATION_REQUIRED if not.
# see: https://irq5.io/2011/06/26/gnu-patch-and-windows-uac/

install.exe: ${SRCDIR}/usr.bin/xinstall/xinstall.c
	gcc -s -static ${CFLAGS} -DNOPIE -o $@ $<

grep: grep.c
	@gcc -c ${CFLAGS} -o binary.o ${SRCDIR}/usr.bin/grep/binary.c
	@gcc -c ${CFLAGS} -o file.o ${SRCDIR}/usr.bin/grep/file.c
	@gcc -c ${CFLAGS} -o mmfile.o ${SRCDIR}/usr.bin/grep/mmfile.c
	@gcc -c ${CFLAGS} -o queue.o ${SRCDIR}/usr.bin/grep/queue.c
	@gcc -c ${CFLAGS} -o util.o ${SRCDIR}/usr.bin/grep/util.c
	gcc -s ${CFLAGS} -I${SRCDIR}/usr.bin/grep -o $@ $< binary.o file.o mmfile.o queue.o util.o -lz

du: ${SRCDIR}/usr.bin/du/du.c
	gcc -s ${CFLAGS} -o $@ $< -lutil

env: env.c
	gcc -s ${CFLAGS} -o $@ $<

install-local: ${UBINDIR}

install: ${DESTDIR} ${OBJECTS} install.exe
	@mkdir -p ${DESTDIR}/usr/bin
	${INSTALL} ${OBJECTS} install install.manifest ${DESTDIR}/usr/bin/

clean:
	/bin/rm -f *.exe *.o
