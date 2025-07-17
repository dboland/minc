ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

HEADERS= ${INCDIR}/pthread.h

${INCDIR}/pthread.h:
	@/bin/cp -vu ${SRCDIR}/lib/libpthread/include/pthread.h $@

install-local: ${HEADERS}
