include ../Makefile.inc

RELEASE=6.1.0

LDFLAGS= -s -Wl,--disable-auto-import -Wl,--enable-stdcall-fixup
GCCLIB=/lib/gcc/mingw32/4.8.1
WINLIB=/C/WINDOWS/system32
PROGS=wrc mkent wtrace mount_refs
WINDIR=/C/WINDOWS/system32
TRACELIB=../libtrace/libtrace.a

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

#
# use  -mwindows -L${WINDIR} for a Windows app
#

bsd.exe: bsd.o version.o
	gcc -static ${LDFLAGS} -o $@ $^

console.exe: console.o libconsole.o
	gcc ${LDFLAGS} -L${WINDIR} -o $@ console.o libconsole.o -lutil

mount_refs: ${SRCDIR}/sbin/mount_ntfs/mount_ntfs.c
	gcc -c ${CFLAGS} -I../include -I../openbsd/include -o $@ $<

wrc: wrc.exe

libwtrace.o: libwtrace.c
	gcc -c ${CFLAGS} -I${MINGW}/include -I../include -I../libtrace -o $@ $<

wtrace: wtrace.c libwtrace.o ${TRACELIB}
	gcc -s ${CFLAGS} ${LDFLAGS} -I../include -o $@ $^ \
	${MINGW}/lib/libadvapi32.a ${MINGW}/lib/libkernel32.a ${MINGW}/lib/libuser32.a \
	${MINGW}/lib/libntdll.a

install-wtrace:
	ginstall wtrace ${DESTDIR}${SBINDIR}/

install: ${DESTDIR}
	@for file in ${PROGS}; do echo $$file; ginstall $$file "${DESTDIR}${SBINDIR}/$$file"; done
	ginstall -m 02555 -o 0 -g 0 bsd.exe ${DESTDIR}${SBINDIR}/

clean:
	/bin/rm -f *.exe *.o

