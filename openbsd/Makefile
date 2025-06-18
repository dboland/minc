include ../Makefile.inc

all:
	@${MAKE} -C lib
	@${MAKE} -C usr.bin

${SRCDIR}:
	@echo "| OpenBSD source directory ($@) not found. Make sure"
	@echo "| it is defined correctly in Makefile.inc"
	@exit 1

includes-local: ${SRCDIR}
	@/bin/mkdir -p ${INCDIR}/machine
	/bin/cp -ru ${SRCDIR}/include/* ${INCDIR}/
	/bin/cp -ru ${SRCDIR}/sys/sys ${INCDIR}/
	/bin/cp -ru ${SRCDIR}/sys/arch/i386/include/* ${INCDIR}/machine/
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
