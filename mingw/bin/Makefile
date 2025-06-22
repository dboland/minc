include ../../Makefile.inc

OBJECTS=ginstall.exe
#OBJECTS+=cat.exe scacls.exe
COREDIR=/D/src/coreutils-8.13/src

# cat.exe: standard output: Bad file descriptor

all: ${OBJECTS} install-local

%.exe: ${COREDIR}/%.exe
	/bin/install -s $< $@

scacls.exe: scacls.c
	gcc -s -static ${CFLAGS} -I../../include -o $@ $<

install-local:
	/bin/cp -rvu ${OBJECTS} ${PREFIX}/local/bin/

install:
	ginstall ginstall.exe ${DESTDIR}/usr/bin/ginstall

clean:
	/bin/rm -f ${OBJECTS}
