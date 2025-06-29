include ../../Makefile.inc

OBJECTS= arch uname touch
CFLAGS+= -I${SRCDIR}/sys
LBINDIR= ${PREFIX}/local/bin
UBINPATH= ${SRCDIR}/usr.bin

all: ${UBINPATH} ${OBJECTS} install.exe grep

${UBINPATH}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'usr.bin' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

${LBINDIR}: ${OBJECTS}
	@/bin/mkdir -p ${LBINDIR}
	/bin/cp -vu ${OBJECTS} install.exe.manifest ${LBINDIR}/
	/bin/cp -vu install.exe ${LBINDIR}/

uname: ${SRCDIR}/usr.bin/uname/uname.c
	gcc -s -static ${CFLAGS} -DNOPIE -o $@ $<

arch: ${SRCDIR}/usr.bin/arch/arch.c
	gcc -s -static ${CFLAGS} -DNOPIE -o $@ $<

touch: ${SRCDIR}/usr.bin/touch/touch.c
	gcc -s -static ${CFLAGS} -DNOPIE -o $@ $<

grep: grep.c
	@gcc -c ${CFLAGS} -o binary.o ${SRCDIR}/usr.bin/grep/binary.c
	@gcc -c ${CFLAGS} -o file.o ${SRCDIR}/usr.bin/grep/file.c
	@gcc -c ${CFLAGS} -o mmfile.o ${SRCDIR}/usr.bin/grep/mmfile.c
	@gcc -c ${CFLAGS} -o queue.o ${SRCDIR}/usr.bin/grep/queue.c
	@gcc -c ${CFLAGS} -o util.o ${SRCDIR}/usr.bin/grep/util.c
	gcc -s ${CFLAGS} -o $@ $< binary.o file.o mmfile.o queue.o util.o -lz

# In Windows, "install" seems to be a reserved word. When 
# executed with such name, Vista checks if user is elevated, 
# emits ERROR_ELEVATION_REQUIRED if not.
# see: https://irq5.io/2011/06/26/gnu-patch-and-windows-uac/

install.exe: ${SRCDIR}/usr.bin/xinstall/xinstall.c
	gcc -s -static ${CFLAGS} -DNOPIE -o $@ $<

install-local: ${LBINDIR}

install: ${DESTDIR}
	@mkdir -p ${DESTDIR}${UBINDIR}
	${INSTALL} ${OBJECTS} install install.manifest grep ${DESTDIR}${UBINDIR}/

clean:
	/bin/rm -f *.exe *.o
