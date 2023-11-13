include ../Makefile.inc

all:
	@${MAKE} -C lib
	@${MAKE} -C usr.bin

includes-local: ${INCDIR}/machine
	@/bin/cp -rvu include/* ${INCDIR}/
	@/bin/cp -rvu sys/sys ${INCDIR}/
	@/bin/cp -rvu sys/dev ${INCDIR}/
	@/bin/cp -rvu sys/net ${INCDIR}/
	@/bin/cp -rvu sys/nfs ${INCDIR}/
	@/bin/cp -rvu sys/uvm ${INCDIR}/
	@/bin/cp -rvu sys/netmpls ${INCDIR}/
	@/bin/cp -rvu sys/netinet ${INCDIR}/
	@/bin/cp -rvu sys/netinet6 ${INCDIR}/
	@/bin/cp -rvu sys/net80211 ${INCDIR}/
	@/bin/cp -rvu sys/arch/i386/include/* ${INCDIR}/machine/
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
