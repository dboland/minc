include ../../Makefile.inc

all:
	@${MAKE} -C libc all install-local
	@${MAKE} -C libdl all install-local
	@${MAKE} -C libsocket all install-local
	@${MAKE} -C libutil all install-local

clean:
	@${MAKE} -C libc clean
	@${MAKE} -C libdl clean
	@${MAKE} -C libsocket clean
	@${MAKE} -C libutil clean

install:
	@${MAKE} -C libc install
	@${MAKE} -C libdl install
	@${MAKE} -C libsocket install
