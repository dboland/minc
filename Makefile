usage:
	@echo "usage: ${MAKE} TARGET"
	@echo
	@echo "Targets:"
	@echo " opt			set up a simple cross-compiler"
	@echo " all			build all targets"
	@echo " kernel			build libtrace and libposix"
	@echo " system			build OpenBSD system binaries"
	@echo " release		update version info in shared libraries"
	@echo " install		install OpenBSD into 'DESTDIR'"
	@echo " help			show build instructions"

all: kernel system

%.d: %
	@${MAKE} -C $<

opt: config.inc
	@${MAKE} -C openbsd install-local
	@${MAKE} -C mingw all install-local

kernel:
	@${MAKE} -C libtrace all
	@${MAKE} -C libposix all install-local

system:
	@${MAKE} -C openbsd

config.inc:
	@/bin/cp $@.sample $@

help:
	@less BUILD.md

install:
	@${MAKE} -C libposix install
	@${MAKE} -C openbsd install

release:
	@${MAKE} -C libposix release
	@${MAKE} -C openbsd release

clean:
	@${MAKE} -C mingw clean
	@${MAKE} -C libtrace clean
	@${MAKE} -C libposix clean
	@${MAKE} -C openbsd clean
