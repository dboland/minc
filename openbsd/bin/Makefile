include ../../Makefile.inc

OBJECTS=chmod.exe ln.exe arch.exe touch.exe \
	mkdir.exe mv.exe rm.exe rmdir.exe uname.exe \
	test.exe mknod.exe chown.exe chgrp.exe \
	install.exe ls.exe cp.exe
CFLAGS+=-DNOPIE -I${SRCDIR}/sys
LBINDIR= ${PREFIX}/local/bin

all: ${OBJECTS}

${LBINDIR}: ${OBJECTS}
	@/bin/mkdir -p ${LBINDIR}
	/bin/cp -vu ${OBJECTS} ${LBINDIR}/

uname.exe: ${SRCDIR}/usr.bin/uname/uname.c
	gcc -s -static ${CFLAGS} -o $@ $<

arch.exe: ${SRCDIR}/usr.bin/arch/arch.c
	gcc -s -static ${CFLAGS} -o $@ $<

touch.exe: ${SRCDIR}/usr.bin/touch/touch.c
	gcc -s -static ${CFLAGS} -o $@ $<

mknod.exe: ${SRCDIR}/sbin/mknod/mknod.c
	gcc -s -static ${CFLAGS} -o $@ $<

chmod.exe: ${SRCDIR}/bin/chmod/chmod.c
	gcc -s -static ${CFLAGS} -o $@ $<

chown.exe: chmod.exe
	/bin/cp -vu $< $@

chgrp.exe: chmod.exe
	/bin/cp -vu $< $@

ln.exe: ${SRCDIR}/bin/ln/ln.c
	gcc -s -static ${CFLAGS} -o $@ $<

mkdir.exe: ${SRCDIR}/bin/mkdir/mkdir.c
	gcc -s -static ${CFLAGS} -o $@ $<

mv.exe: ${SRCDIR}/bin/mv/mv.c
	gcc -s -static ${CFLAGS} -o $@ $<

rm.exe: ${SRCDIR}/bin/rm/rm.c
	gcc -s -static ${CFLAGS} -o $@ $<

rmdir.exe: ${SRCDIR}/bin/rmdir/rmdir.c
	gcc -s -static ${CFLAGS} -o $@ $<

test.exe: ${SRCDIR}/bin/test/test.c
	gcc -s -static ${CFLAGS} -o $@ $<

install.exe: ${SRCDIR}/usr.bin/xinstall/xinstall.c
	gcc -s -static ${CFLAGS} -o $@ $<

ls.exe:
	@${MAKE} -C ls CFLAGS="${CFLAGS}" SRCDIR="${SRCDIR}"

cp.exe:
	@${MAKE} -C cp CFLAGS="${CFLAGS}" SRCDIR="${SRCDIR}"

install-local: ${LBINDIR}

clean:
	/bin/rm -f ${OBJECTS}
