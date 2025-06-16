include ../Makefile.inc

#LINKS=include/fcntl.h include/sched.h include/stdarg.h \
#	include/stdint.h include/termios.h

LINKS=include/stdarg.h include/stdint.h

all:
	@${MAKE} -C lib
	@${MAKE} -C usr.bin

${SRCDIR}:
	@echo "| OpenBSD source directory ($@) not found. Make sure"
	@echo "| it is defined correctly in Makefile.inc"
	@exit 1

includes-local: ${SRCDIR}
	@/bin/mkdir -p ${INCDIR}/machine ${INCDIR}/dev
	/bin/cp -ru ${SRCDIR}/include/* ${INCDIR}/
	/bin/cp -ru ${SRCDIR}/sys/sys ${INCDIR}/
	/bin/cp -ru ${SRCDIR}/sys/arch/i386/include/* ${INCDIR}/machine/
	@#/bin/cp -ru ${SRCDIR}/sys/dev/wscons ${INCDIR}/dev/
	@#/bin/cp -ru ${SRCDIR}/sys/netinet6 ${INCDIR}/
	@#/bin/cp -ru ${SRCDIR}/sys/uvm ${INCDIR}/
	@#/bin/cp -ru ${SRCDIR}/sys/net ${INCDIR}/
	@#/bin/cp -ru ${SRCDIR}/sys/netinet ${INCDIR}/
	@#/bin/cp -ru ${LINKS} ${INCDIR}/
	@${MAKE} -C lib includes-local

includes: ${DESTDIR}
	@cp -rvu include/* ${DESTDIR}${INCDIR}/
	@cp -rvu sys/sys ${DESTDIR}${INCDIR}/
	@cp -rvu sys/arch/i386/include/* ${DESTDIR}${INCDIR}/machine/

install: ${DESTDIR}
	@cp -rvu share/mk ${DESTDIR}${SHDIR}/
	@${MAKE} -C lib install

install-local:
	@${MAKE} -C lib install-local
	@${MAKE} -C usr.bin install-local

clean:
	@${MAKE} -C lib clean
	@${MAKE} -C usr.bin clean
