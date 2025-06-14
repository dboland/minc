include ../Makefile.inc

ARCHDIR=sys/arch/i386/include

all:
	@${MAKE} -C lib
	@${MAKE} -C usr.bin

includes-local:
	@/bin/mkdir -p ${INCDIR}/machine
	@/bin/cp -ru ${SRCDIR}/include/* ${INCDIR}/
	@/bin/cp -rvu include/* ${INCDIR}/
	@/bin/cp -ru ${SRCDIR}/sys/sys ${INCDIR}/
	@/bin/cp -rvu sys/sys ${INCDIR}/
	@/bin/cp -rvu sys/netinet6 ${INCDIR}/
	@/bin/cp -ru ${SRCDIR}/${ARCHDIR}/* ${INCDIR}/machine/
	@/bin/cp -rvu ${ARCHDIR}/* ${INCDIR}/machine/
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
