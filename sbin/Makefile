include ../Makefile.inc

RELEASE=6.1.0
BINDIR=/sbin

CFLAGS= -c -fno-omit-frame-pointer -Wimplicit-function-declaration
LDFLAGS= -Wl,--disable-auto-import -L../libposix
MINGWLIB=/C/MinGW/lib
GCCLIB=/lib/gcc/mingw32/4.8.1
WINLIB=/C/WINDOWS/system32
PROGS=wrc mkent
LIBS=-lposix

.SUFFIXES:
.DEFAULT:

all: ${PROGS} bsd.exe

%.o: %.c
	gcc ${CFLAGS} -I../openbsd/include -I../include -I../openbsd/sys -o $@ $<

%.exe: %.o
	gcc -s ${LDFLAGS} -o $@ $< ${LIBS}

#
# -Wl,-o inhibits the default --force-exe-suffix.
#

version.o: bsd.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

#
# -Wl,--subsystem,windows for app without console
#

mkent: mkent.o
	gcc -o $@ $< ${LDFLAGS} ${MINGWLIB}/libiphlpapi.a ${LIBS}

mtrace: mtrace.c
	gcc ${CFLAGS} -mtls-direct-seg-refs -I../include -o mtrace.o $<
	gcc ${LDFLAGS} -Wl,--image-base=1048576 -o $@ mtrace.o -lgcc_eh

bsd.exe: bsd.o version.o
	gcc -o $@ $^ ${LDFLAGS} ${LIBS}

wrc: wrc.exe

install-local:
	./mkent passwd >/etc/passwd
	./mkent group >/etc/group

install: ${DESTDIR}
	@#cp -vu ${PROGS} ${DESTDIR}${BINDIR}/
	@for file in ${PROGS}; do echo $$file; ginstall $$file "${DESTDIR}${BINDIR}/$$file"; done
	ginstall -m 04555 bsd.exe ${DESTDIR}/

clean:
	rm -f *.exe *.o

