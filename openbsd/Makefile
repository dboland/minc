ROOTDIR= ..

include ${ROOTDIR}/Makefile.inc

HEADERS= ${GINCDIR}/fcntl.h ${GINCDIR}/stdarg.h ${GINCDIR}/stdint.h \
	${GINCDIR}/sched.h ${GINCDIR}/syslog.h ${GINCDIR}/termios.h
MACHINEDIR= ${GINCDIR}/machine

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
	@echo "| the 'SRCDIR' variable is defined correctly in config.inc"
	@echo " ---------------------------------------------------"
	@exit 1

${ROOTDIR}/config.inc:
	@echo " ---------------------------------------------------"
	@echo "| Cross-compiler not set up. Please run 'make opt' first."
	@echo " ---------------------------------------------------"
	@exit 1

${MACHINEDIR}:
	@/bin/mkdir -p ${MACHINEDIR}
	/bin/cp -ru ${SRCDIR}/include/* ${GINCDIR}/
	/bin/cp -ru ${SRCDIR}/sys/sys ${GINCDIR}/
	/bin/cp -ru ${SRCDIR}/sys/arch/${ARCH}/include/* ${MACHINEDIR}/
	/bin/patch -p 0 -d ${GPREFIX} <include.patch

${GINCDIR}/fcntl.h:
	@/bin/cp -vu fcntl.h ${GINCDIR}/

${GINCDIR}/sched.h:
	@/bin/cp -vu sched.h ${GINCDIR}/

${GINCDIR}/stdarg.h:
	@/bin/cp -vu stdarg.h ${GINCDIR}/

${GINCDIR}/stdint.h:
	@/bin/cp -vu stdint.h ${GINCDIR}/

${GINCDIR}/syslog.h:
	@/bin/cp -vu syslog.h ${GINCDIR}/

${GINCDIR}/termios.h:
	@/bin/cp -vu termios.h ${GINCDIR}/

${XINCDIR}:
	@/bin/mkdir -p ${XINCDIR}
	/bin/cp -ru ${GINCDIR}/* ${XINCDIR}/

install-local: ${SRCDIR} ${MACHINEDIR} ${HEADERS}

install: ${DESTDIR}
	@${MAKE} -C lib install
	@${MAKE} -C bin install
	@${MAKE} -C usr.bin install
	@${MAKE} -C sbin install
	@${MAKE} -C usr.sbin install
	@${MAKE} -C etc install

install-cross: ${XINCDIR}
	@#${MAKE} -C share install
	@${MAKE} -C lib install-cross

release:
	@${MAKE} -C lib release
	@${MAKE} -C sbin release
	
clean:
	@${MAKE} -C lib clean
	@${MAKE} -C bin clean
	@${MAKE} -C usr.bin clean
	@${MAKE} -C sbin clean
	@${MAKE} -C usr.sbin clean
