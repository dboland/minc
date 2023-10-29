include ../../Makefile.inc

all:
	@${MAKE} -C libc
	@${MAKE} -C libd
	@${MAKE} -C libsocket

includes-local:
	@/bin/cp -rvu include/* ${INCDIR}/

install-local:
	@${MAKE} -C libc install-local
	@${MAKE} -C libd install-local
	@${MAKE} -C libsocket install-local

clean:
	@${MAKE} -C libc clean
	@${MAKE} -C libd clean
	@${MAKE} -C libsocket clean

install:
	@${MAKE} -C libc install
	@${MAKE} -C libd install
	@${MAKE} -C libsocket install
