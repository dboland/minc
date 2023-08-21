.DEFAULT:
.SUFFIXES:

all: Makefile.inc mount.sh
	@${MAKE} -C mingw/gcc
	@${MAKE} -C libposix

Makefile.inc:
	cp Makefile.inc.sample Makefile.inc

mount.sh:
	cp mount.sh.sample mount.sh

clean:
	${MAKE} -C libposix clean
	${MAKE} -C mingw clean
	${MAKE} -C sbin clean
