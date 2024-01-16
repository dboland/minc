include ../Makefile.inc

RELEASE=6.1.0
SBINDIR=/sbin

LDFLAGS= -s -Wl,--disable-auto-import -L../libposix
GCCLIB=/lib/gcc/mingw32/4.8.1
WINLIB=/C/WINDOWS/system32
PROGS=wrc mkent
LIBS=-lposix

.SUFFIXES:
.DEFAULT:

all: ${PROGS} bsd.exe

%.o: %.c
	gcc -c ${CFLAGS} -I../openbsd/include -I../include -I../openbsd/sys -o $@ $<

%.exe: %.o
	gcc ${LDFLAGS} -o $@ $< ${LIBS}

#
# -Wl,-o inhibits the default --force-exe-suffix.
#

version.o: bsd.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

mkent: mkent.o
	gcc ${LDFLAGS} -o $@ $< ${MINGW}/lib/libiphlpapi.a ${LIBS}

bsd.exe: bsd.o version.o
	gcc ${LDFLAGS} -o $@ $^ ${LIBS}

wrc: wrc.exe

install-local:
	./mkent passwd >/etc/passwd
	./mkent group >/etc/group

install: ${DESTDIR}
	@for file in ${PROGS}; do echo $$file; ginstall $$file "${DESTDIR}${SBINDIR}/$$file"; done
	ginstall -m 06555 -o 0 -g 0 bsd.exe ${DESTDIR}/

clean:
	/bin/rm -f *.exe *.o

