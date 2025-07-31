ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

HEADERS= ${GINCDIR}/pthread.h

${GINCDIR}/pthread.h:
	@/bin/cp -vu ${SRCDIR}/lib/libpthread/include/pthread.h $@

install-local: ${HEADERS}
