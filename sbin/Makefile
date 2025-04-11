include ../Makefile.inc

RELEASE=6.1.0

LDFLAGS= -s -Wl,--disable-auto-import -Wl,--enable-stdcall-fixup
GCCLIB=/lib/gcc/mingw32/4.8.1
WINLIB=/C/WINDOWS/system32
PROGS=wrc mkent
WINDIR=/C/WINDOWS/system32

.SUFFIXES:
.DEFAULT:

all: ${PROGS} bsd.exe

bsd.o: bsd.c
	gcc -c -DNOPIE ${CFLAGS} -I../include -I../openbsd/sys -o $@ $<

%.o: %.c
	gcc -c ${CFLAGS} -I../include -I../openbsd/sys -o $@ $<

%.exe: %.o
	gcc ${LDFLAGS} -o $@ $< ${LIBS}

#
# -Wl,-o inhibits the default --force-exe-suffix.
#

version.o: bsd.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

mkent: mkent.o
	gcc ${LDFLAGS} -o $@ $< ${MINGW}/lib/libiphlpapi.a

libconsole.o: libconsole.c
	gcc -c ${CFLAGS} -I${MINGW}/include -I../include -o $@ $<

#
# use  -mwindows -L${WINDIR} for a Windows app
#

bsd.exe: bsd.o version.o
	gcc -static ${LDFLAGS} -o $@ $^

console.exe: console.o libconsole.o
	gcc ${LDFLAGS} -L${WINDIR} -o $@ console.o libconsole.o -lutil

wrc: wrc.exe

install-local:
	./mkent passwd >/etc/passwd
	./mkent group >/etc/group

install: ${DESTDIR}
	@for file in ${PROGS}; do echo $$file; ginstall $$file "${DESTDIR}${SBINDIR}/$$file"; done
	ginstall -m 02555 -o 0 -g 0 bsd.exe ${DESTDIR}${SBINDIR}/

clean:
	/bin/rm -f *.exe *.o

