include ../../Makefile.inc

OBJECTS=chmod.exe chgrp.exe cp.exe ln.exe ls.exe \
	mkdir.exe mv.exe rm.exe rmdir.exe touch.exe uname.exe chown.exe \
	ginstall.exe mknod.exe test.exe scacls.exe
#OBJECTS+=cat.exe
COREDIR=/D/src/coreutils-8.13/src
CFLAGS+=-s

# cat.exe: standard output: Bad file descriptor

all: ${OBJECTS}

%.exe: ${COREDIR}/%.exe
	/bin/install -s $< $@

scacls.exe: scacls.c
	gcc ${CFLAGS} -I../../include -o $@ $<

install:
	ginstall ginstall.exe ${DESTDIR}/usr/bin/ginstall
