include Makefile.inc

usage:
	@echo "usage: ${MAKE} TARGET"
	@echo
	@echo "Targets:"
	@echo " all			build all targets"
	@echo " kernel			build libposix only"
	@echo " system			build OpenBSD system binaries only"
	@echo " install		install OpenBSD into ${DESTDIR}"
	@echo " help			show build instructions"

${DESTDIR}:
	@echo " ---------------------------------------------------"
	@echo "| MinC install directory ($@) not found. Make sure"
	@echo "| it exists and the 'DESTDIR' variable is defined correctly in Makefile.inc"
	@echo " ---------------------------------------------------"
	@exit 1

all: kernel system

%.d: %
	@${MAKE} -C $<

kernel: Makefile.inc mount.sh
	@${MAKE} -C openbsd includes-local
	@${MAKE} -C mingw all install-local
	@${MAKE} -C libtrace all
	@${MAKE} -C libposix all install-local

system:
	@${MAKE} -C openbsd

Makefile.inc:
	@/bin/cp Makefile.inc.sample Makefile.inc

mount.sh:
	@/bin/cp mount.sh.sample mount.sh

help:
	@/bin/less BUILD.md

clean:
	@${MAKE} -C libtrace clean
	@${MAKE} -C libposix clean
	@${MAKE} -C mingw clean
	@${MAKE} -C openbsd clean

install: ${DESTDIR}
	@${MAKE} -C libposix install
	@${MAKE} -C openbsd install
