ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

PROGS= arch uname touch
OBJECTS= ${PROGS} grep du env
CFLAGS+= -I${SRCDIR}/sys
UBINSRC= ${SRCDIR}/usr.bin
UBINDIR= /usr/bin

# GNU make can't cd into make if make/make.exe exists?!?!

all: ${UBINSRC} ${PROGS} install.exe patch.exe gzip.exe
	@${MAKE} -C make UBINSRC="${UBINSRC}"

${UBINSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'usr.bin' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

${BINDIR}: ${PROGS} install.exe patch.exe gzip.exe
	@/bin/mkdir -p ${BINDIR}
	/bin/cp -vu $^ ${BINDIR}/
	/bin/cp -vu manifest ${BINDIR}/install.exe.manifest
	/bin/cp -vu manifest ${BINDIR}/patch.exe.manifest

uname: ${UBINSRC}/uname/uname.c
	gcc -s -static ${CFLAGS} -DNOPIE -o $@ $<

arch: ${UBINSRC}/arch/arch.c
	gcc -s -static ${CFLAGS} -DNOPIE -o $@ $<

touch: ${UBINSRC}/touch/touch.c
	gcc -s -static ${CFLAGS} -DNOPIE -o $@ $<

# In Windows, "install" and "patch" seem to be reserved 
# words. When executed with such name, Vista checks if 
# user is elevated, yields ERROR_ELEVATION_REQUIRED if not.
# see: https://irq5.io/2011/06/26/gnu-patch-and-windows-uac/

install.exe: xinstall.c
	gcc -s -static ${CFLAGS} -DNOPIE -I${UBINSRC}/xinstall -o $@ $<

patch.exe: ${UBINSRC}/patch/patch.c ${UBINSRC}/patch/pch.c ${UBINSRC}/patch/inp.c \
	${UBINSRC}/patch/util.c ${UBINSRC}/patch/backupfile.c ${UBINSRC}/patch/mkpath.c
	gcc -s -static ${CFLAGS} -DNOPIE -o $@ $^

gzip.exe: ${UBINSRC}/compress/main.c ${UBINSRC}/compress/zopen.c \
	${UBINSRC}/compress/gzopen.c ${UBINSRC}/compress/nullopen.c
	gcc -s -static ${CFLAGS} -DNOPIE -o $@ $^ -lz

grep: grep.c ${UBINSRC}/grep/binary.c ${UBINSRC}/grep/file.c ${UBINSRC}/grep/mmfile.c \
	${UBINSRC}/grep/queue.c ${UBINSRC}/grep/util.c
	gcc -s ${CFLAGS} -I${UBINSRC}/grep -o $@ $^ -lz

du: ${UBINSRC}/du/du.c
	gcc -s ${CFLAGS} -o $@ $< -lutil

env: env.c
	gcc -s ${CFLAGS} -o $@ $<

install-local: ${BINDIR}

install: ${DESTDIR} ${OBJECTS}
	@mkdir -p ${DESTDIR}${UBINDIR}
	${INSTALL} ${OBJECTS} ${DESTDIR}${UBINDIR}/
	${INSTALL} manifest ${DESTDIR}${UBINDIR}/install.manifest
	${INSTALL} manifest ${DESTDIR}${UBINDIR}/patch.manifest
	@${MAKE} -C make install UBINDIR="${UBINDIR}"

clean:
	/bin/rm -f *.exe *.o
	@${MAKE} -C make clean
