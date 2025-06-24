.DEFAULT:
.SUFFIXES:

usage:
	@echo "usage: ${MAKE} TARGET"
	@echo
	@echo "Targets:"
	@echo " all			build all targets"
	@echo " kernel			build libposix only"
	@echo " system			build OpenBSD system libraries only"
	@echo " help			show build instructions"

all: kernel system

%.d: %
	@${MAKE} -C $<

kernel: Makefile.inc mount.sh
	@${MAKE} -C openbsd includes-local
	@${MAKE} -C mingw all install-local
	@${MAKE} -C libtrace all
	@${MAKE} -C libposix all install-local

system:
	@${MAKE} -C openbsd all install-local
	@${MAKE} -C sbin all

Makefile.inc:
	@/bin/cp Makefile.inc.sample Makefile.inc

mount.sh:
	@/bin/cp mount.sh.sample mount.sh

help:
	@/bin/cat BUILD.md

clean:
	@${MAKE} -C libtrace clean
	@${MAKE} -C libposix clean
	@${MAKE} -C mingw clean
	@${MAKE} -C sbin clean
	@${MAKE} -C openbsd clean

install:
	@${MAKE} -C libposix install
	@${MAKE} -C openbsd install
	@${MAKE} -C sbin install
