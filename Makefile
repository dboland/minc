.DEFAULT:
.SUFFIXES:

usage: Makefile.inc mount.sh
	@echo "usage: make TARGET"
	@echo
	@echo "Targets:"
	@echo " all			build all targets"
	@echo " kernel			build libposix only"
	@echo " libs			build BSD system libraries only"
	@echo " includes		install header files locally"
	@echo " sbin			buid MinC system programs only"

%.d: %
	@${MAKE} -C $<

all:
	@${MAKE} -C openbsd includes-local
	@${MAKE} -C mingw all install-local
	@${MAKE} -C libtrace all
	@${MAKE} -C libposix all install-local
	@${MAKE} -C openbsd all install-local
	@${MAKE} -C sbin all

kernel:
	@${MAKE} -C libposix all install-local

includes:
	@${MAKE} -C openbsd includes-local

libs: openbsd.d

sbin: libtrace.d
	@${MAKE} -C sbin all install

Makefile.inc:
	/bin/cp Makefile.inc.sample Makefile.inc

mount.sh:
	/bin/cp mount.sh.sample mount.sh

clean:
	@${MAKE} -C libtrace clean
	@${MAKE} -C libposix clean
	@${MAKE} -C mingw clean
	@${MAKE} -C sbin clean
	@${MAKE} -C openbsd clean

install:
	@${MAKE} -C libposix install
	@${MAKE} -C openbsd install
