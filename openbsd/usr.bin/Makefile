include ../../Makefile.inc

OBJECTS= arch uname install.exe touch
CFLAGS+=-DNOPIE -I${SRCDIR}/sys
LBINDIR= ${PREFIX}/local/bin
UBINPATH= ${SRCDIR}/usr.bin

all: ${UBINPATH} ${OBJECTS}

${UBINPATH}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'usr.bin' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

${LBINDIR}: ${OBJECTS}
	@/bin/mkdir -p ${LBINDIR}
	/bin/cp -vu ${OBJECTS} ${LBINDIR}/

uname: ${SRCDIR}/usr.bin/uname/uname.c
	gcc -s -static ${CFLAGS} -o $@ $<

arch: ${SRCDIR}/usr.bin/arch/arch.c
	gcc -s -static ${CFLAGS} -o $@ $<

touch: ${SRCDIR}/usr.bin/touch/touch.c
	gcc -s -static ${CFLAGS} -o $@ $<

install.exe: ${SRCDIR}/usr.bin/xinstall/xinstall.c
	gcc -s -static ${CFLAGS} -o $@ $<

install-local: ${LBINDIR}

clean:
	/bin/rm -f *.exe *.o
