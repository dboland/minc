include ../Makefile.inc

LDFLAGS+= -s
PROGS=wrc mkent wtrace mount_refs
TRACEDIR=../libtrace

.SUFFIXES:
.DEFAULT:

all: ${PROGS} bsd.exe

bsd.o: bsd.c
	gcc -c -DNOPIE ${CFLAGS} -I../include -I${SRCDIR}/sys -o $@ $<

%.o: %.c
	gcc -c ${CFLAGS} -I../include -I${SRCDIR}/sys -o $@ $<

%.exe: %.o
	gcc -Wl,--disable-auto-import -o $@ $< ${LIBS}

#
# -Wl,-o inhibits the default --force-exe-suffix.
#

version.o: bsd.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

mkent: mkent.o
	gcc ${LDFLAGS} -o $@ $< ${MINGW}/lib/libiphlpapi.a

#
# use  -mwindows -L${MINGW}/lib for a Windows app
#

bsd.exe: bsd.o version.o
	gcc -static -Wl,--disable-auto-import -o $@ $^

console.exe: console.o libconsole.o
	gcc ${LDFLAGS} -L${MINGW}/lib -o $@ console.o libconsole.o -lutil

mount_refs: ${SRCDIR}/sbin/mount_ntfs/mount_ntfs.c
	gcc -c ${CFLAGS} -I../include -o $@ $<

wrc: wrc.exe

libwtrace.o: libwtrace.c
	gcc -c ${CFLAGS} -I${MINGW}/include -I../include -o $@ $<

wtrace: wtrace.o libwtrace.o
	gcc -s ${CFLAGS} -o $@ -L${TRACEDIR} $^ -ltrace \
	${MINGW}/lib/libkernel32.a ${MINGW}/lib/libuser32.a ${MINGW}/lib/libadvapi32.a

install: ${DESTDIR}
	@for file in ${PROGS}; do echo $$file; install $$file "${DESTDIR}${SBINDIR}/$$file"; done
	install -m 02555 -o 0 -g 0 bsd.exe ${DESTDIR}${SBINDIR}/

clean:
	/bin/rm -f *.exe *.o

