include ../../Makefile.inc

all:
	@${MAKE} -C libc
	@${MAKE} -C libdl
	@${MAKE} -C libsocket
	@${MAKE} -C libutil

includes-local:
	@/bin/cp -rvu include/* ${INCDIR}/

install-local:
	@${MAKE} -C libc install-local
	@${MAKE} -C libdl install-local
	@${MAKE} -C libsocket install-local
	@${MAKE} -C libutil install-local

clean:
	@${MAKE} -C libc clean
	@${MAKE} -C libdl clean
	@${MAKE} -C libsocket clean
	@${MAKE} -C libutil clean

install:
	@${MAKE} -C libc install
	@${MAKE} -C libdl install
	@${MAKE} -C libsocket install
