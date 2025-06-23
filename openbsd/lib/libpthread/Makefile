include ../../../Makefile.inc

HEADERS= pthread.h

%.h: ${SRCDIR}/lib/librthread/%.h
	@/bin/cp -vu $< ${INCDIR}/

install-local: ${HEADERS}
