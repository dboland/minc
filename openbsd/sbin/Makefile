ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

PROGS= wrc mkent wtrace mount_refs mknod
TRACEDIR= ${ROOTDIR}/libtrace
SBINSRC= ${SRCDIR}/sbin
SBINDIR= /sbin

all: ${SBINSRC} ${PROGS} bsd.exe

${SBINSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'sbin' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

%.o: %.c
	gcc -c ${CFLAGS} -I${ROOTDIR}/include -I${SRCDIR}/sys -o $@ $<

%.exe: %.o
	gcc -s ${CFLAGS} -o $@ $<

${BINDIR}: ${PROGS}
	@/bin/mkdir -p ${BINDIR}
	/bin/cp -vu mknod ${BINDIR}/

wrc: wrc.exe

mknod: ${SBINSRC}/mknod/mknod.c
	gcc -s -static ${CFLAGS} -DNOPIE -o $@ $<

mkent: mkent.o
	gcc -s ${CFLAGS} -I${SRCDIR}/sys -o $@ $< ${MINGW}/lib/libiphlpapi.a

version.o: bsd.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

bsd.o: bsd.c
	gcc -c ${CFLAGS} -DNOPIE -I${SRCDIR}/sys -o $@ $<

bsd.exe: bsd.o version.o
	gcc -s -static ${CFLAGS} -o $@ $^

libwtrace.o: libwtrace.c
	gcc -c ${CFLAGS} -I${MINGW}/include -I${ROOTDIR}/include -o $@ $<

wtrace: libwtrace.o wtrace.o
	gcc -s ${CFLAGS} -o $@ -L${TRACEDIR} $^ -ltrace \
	${MINGW}/lib/libkernel32.a ${MINGW}/lib/libuser32.a ${MINGW}/lib/libadvapi32.a

mount_refs:
	@gcc -c ${CFLAGS} -I${SBINSRC}/mount -o mount_ntfs.o ${SBINSRC}/mount_ntfs/mount_ntfs.c
	@gcc -c ${CFLAGS} -o getmntopts.o ${SBINSRC}/mount/getmntopts.c
	gcc -s ${CFLAGS} -o $@ mount_ntfs.o getmntopts.o

install-local: ${BINDIR}

install: ${DESTDIR}${SBINDIR}
	@mkdir -p ${DESTDIR}${SBINDIR}
	${INSTALL} ${PROGS} ${DESTDIR}${SBINDIR}/
	${INSTALL} -m 02555 -o 0 -g 0 bsd.exe ${DESTDIR}${SBINDIR}/
	${INSTALL} -m 00754 -o 0 -g 0 setup.sh ${DESTDIR}${SBINDIR}/

clean:
	/bin/rm -f *.exe *.o

