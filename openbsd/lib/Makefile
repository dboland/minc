include ../../Makefile.inc

.DEFAULT:
.SUFFIXES:

all:
	${MAKE} -C libc
	${MAKE} -C libsocket

install-local:
	${MAKE} -C libc install-local
	${MAKE} -C libsocket install-local

clean:
	${MAKE} -C libc clean
	${MAKE} -C libsocket clean
