ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

HEADERS= ${XINCDIR}/pthread.h

${XINCDIR}/pthread.h:
	@/bin/cp -vu ${SRCDIR}/lib/libpthread/include/pthread.h $@

install-local: ${HEADERS}
