include ../Makefile.inc

MACHINEDIR= ${INCDIR}/machine
HEADERS= ${INCDIR}/fcntl.h ${INCDIR}/stdarg.h ${INCDIR}/stdint.h \
	${INCDIR}/sched.h ${INCDIR}/syslog.h ${INCDIR}/termios.h

all: ${SRCDIR}
	@${MAKE} -C lib
	@${MAKE} -C bin all install-local
	@${MAKE} -C usr.bin all install-local
	@${MAKE} -C sbin all install-local
	@${MAKE} -C usr.sbin
	@${MAKE} -C etc

${SRCDIR}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD source directory ($@) not found. Make sure"
	@echo "| the 'SRCDIR' variable is defined correctly in Makefile.inc"
	@echo " ---------------------------------------------------"
	@exit 1

${MACHINEDIR}:
	/bin/mkdir -p ${MACHINEDIR}
	/bin/cp -ru ${SRCDIR}/include/* ${INCDIR}/
	/bin/cp -ru ${SRCDIR}/sys/sys ${INCDIR}/
	/bin/cp -ru ${SRCDIR}/sys/arch/${ARCH}/include/* ${MACHINEDIR}/
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

install-local: ${SRCDIR} ${MACHINEDIR} ${HEADERS}

install: ${DESTDIR}
	@${MAKE} -C lib install
	@${MAKE} -C bin install
	@${MAKE} -C usr.bin install
	@${MAKE} -C sbin install
	@${MAKE} -C usr.sbin install
	@${MAKE} -C etc install

clean:
	@${MAKE} -C lib clean
	@${MAKE} -C bin clean
	@${MAKE} -C usr.bin clean
	@${MAKE} -C sbin clean
	@${MAKE} -C usr.sbin clean
