include ../../Makefile.inc

OBJECTS= arch uname touch install.exe.manifest
CFLAGS+=-DNOPIE -I${SRCDIR}/sys
LBINDIR= ${PREFIX}/local/bin
UBINPATH= ${SRCDIR}/usr.bin

all: ${UBINPATH} ${OBJECTS} install.exe

${UBINPATH}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'usr.bin' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

${LBINDIR}: ${OBJECTS}
	@/bin/mkdir -p ${LBINDIR}
	/bin/cp -vu ${OBJECTS} ${LBINDIR}/
	/bin/cp -vu install.exe ${LBINDIR}/

uname: ${SRCDIR}/usr.bin/uname/uname.c
	gcc -s -static ${CFLAGS} -o $@ $<

arch: ${SRCDIR}/usr.bin/arch/arch.c
	gcc -s -static ${CFLAGS} -o $@ $<

touch: ${SRCDIR}/usr.bin/touch/touch.c
	gcc -s -static ${CFLAGS} -o $@ $<

# In Windows, "install" seems to be a reserved word. When 
# executed with such name, Vista checks if user is elevated, 
# emits ERROR_ELEVATION_REQUIRED if not.
# see: https://irq5.io/2011/06/26/gnu-patch-and-windows-uac/

install.exe: ${SRCDIR}/usr.bin/xinstall/xinstall.c
	gcc -s -static ${CFLAGS} -o $@ $<

install-local: ${LBINDIR}

clean:
	/bin/rm -f *.exe *.o
