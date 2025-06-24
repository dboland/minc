include ../../../Makefile.inc

HEADERS= pthread.h

%.h: ${SRCDIR}/lib/libpthread/include/%.h
	/bin/cp -vu $< ${INCDIR}/

install-local: ${HEADERS}
