.DEFAULT:
.SUFFIXES:

all: Makefile.inc mount.sh
	@${MAKE} -C openbsd includes-local
	@${MAKE} -C mingw all install-local
	@${MAKE} -C libposix all install-local
	@${MAKE} -C openbsd all install-local

Makefile.inc:
	cp Makefile.inc.sample Makefile.inc

mount.sh:
	cp mount.sh.sample mount.sh

clean:
	@${MAKE} -C libposix clean
	@${MAKE} -C mingw clean
	@${MAKE} -C sbin clean
	@${MAKE} -C openbsd clean

install:
	@${MAKE} -C libposix install
	@${MAKE} -C openbsd install
