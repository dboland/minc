include ../Makefile.inc

.DEFAULT:
.SUFFIXES:

all: includes
	${MAKE} -C lib
	${MAKE} -C usr.bin

includes:
	@/bin/cp -rvu include/* ${INCDIR}/

install-local:
	@/bin/cp -vu sys/sys/syscall.h include/sys/
	@#/bin/cp -rvu sys/machine/* ${INCDIR}/machine/
	${MAKE} -C lib install-local
	${MAKE} -C usr.bin install-local

install: ${DESTDIR}
	@cp -rvu include/* ${DESTDIR}${INCDIR}/
	@#cp -rvu sys/machine ${DESTDIR}${INCDIR}/
	@cp -rvu share/mk ${DESTDIR}${SHDIR}/

clean:
	${MAKE} -C lib clean
