include ../../Makefile.inc

OBJECTS=chmod.exe chgrp.exe cp.exe ln.exe ls.exe \
	mkdir.exe mv.exe rm.exe rmdir.exe touch.exe uname.exe chown.exe \
	ginstall.exe mknod.exe test.exe scacls.exe
#OBJECTS+=cat.exe
COREDIR=/D/src/coreutils-8.13/src
CFLAGS+=-s -Wl,--disable-auto-import -static

# cat.exe: standard output: Bad file descriptor

all: ${OBJECTS}

%.exe: ${COREDIR}/%.exe
	/bin/install -s $< $@

uname.exe: ${SRCDIR}/usr.bin/uname/uname.c
	gcc ${CFLAGS} -I${SRCDIR}/include -I${SRCDIR}/sys -o $@ $<

scacls.exe: scacls.c
	gcc ${CFLAGS} -I../../include -o $@ $<

install:
	ginstall ginstall.exe ${DESTDIR}/usr/bin/ginstall
