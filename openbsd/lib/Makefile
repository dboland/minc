include ../../Makefile.inc

.DEFAULT:
.SUFFIXES:

all:
	@${MAKE} -C libc
	@${MAKE} -C libsocket

includes-local:
	@/bin/cp -rvu include/* ${INCDIR}/

install-local:
	@${MAKE} -C libc install-local
	@${MAKE} -C libsocket install-local

clean:
	@${MAKE} -C libc clean
	@${MAKE} -C libsocket clean

install:
	@${MAKE} -C libc install
	@${MAKE} -C libsocket install
