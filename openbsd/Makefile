include ../Makefile.inc

MACHINEDIR= ${INCDIR}/machine
HEADERS= ${INCDIR}/fcntl.h ${INCDIR}/stdarg.h ${INCDIR}/stdint.h \
	${INCDIR}/sched.h ${INCDIR}/syslog.h ${INCDIR}/termios.h

all:
	@${MAKE} -C lib
	@${MAKE} -C bin

${SRCDIR}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD source directory ($@) not found. Make sure"
	@echo "| the 'SRCDIR' variable is defined correctly in Makefile.inc"
	@echo " ---------------------------------------------------"
	@exit 1

${MACHINEDIR}: ${SRCDIR}
	/bin/mkdir -p ${MACHINEDIR}
	/bin/cp -ru ${SRCDIR}/include/* ${INCDIR}/
	/bin/cp -ru ${SRCDIR}/sys/sys ${INCDIR}/
	/bin/cp -ru ${SRCDIR}/sys/arch/i386/include/* ${MACHINEDIR}/
	/bin/patch -p 0 -d ${PREFIX} <include.patch

${INCDIR}/fcntl.h:
	@/bin/cp -vu fcntl.h ${INCDIR}/

${INCDIR}/sched.h:
	@/bin/cp -vu sched.h ${INCDIR}/

${INCDIR}/stdarg.h:
	@/bin/cp -vu stdarg.h ${INCDIR}/

${INCDIR}/stdint.h:
	@/bin/cp -vu stdint.h ${INCDIR}/

${INCDIR}/syslog.h:
	@/bin/cp -vu syslog.h ${INCDIR}/

${INCDIR}/termios.h:
	@/bin/cp -vu termios.h ${INCDIR}/

includes-local: ${MACHINEDIR} ${HEADERS}

includes: ${DESTDIR}
	@cp -rvu include/* ${DESTDIR}${INCDIR}/
	@cp -rvu sys/sys ${DESTDIR}${INCDIR}/
	@cp -rvu sys/arch/i386/include/* ${DESTDIR}${INCDIR}/machine/

install-local:
	@${MAKE} -C bin install-local

install: ${DESTDIR}
	@/bin/cp -rvu share/mk ${DESTDIR}${SHAREDIR}/
	@${MAKE} -C lib install

clean:
	@${MAKE} -C lib clean
	@${MAKE} -C bin clean
