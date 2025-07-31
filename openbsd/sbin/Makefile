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
	${XGCC} -c ${CFLAGS} -I${ROOTDIR}/include -I${SRCDIR}/sys -o $@ $<

%.exe: %.o
	${XGCC} -s ${CFLAGS} -o $@ $<

${BINDIR}: ${PROGS}
	@/bin/mkdir -p ${BINDIR}
	/bin/cp -vu mknod ${BINDIR}/

wrc: wrc.exe

mknod: ${SBINSRC}/mknod/mknod.c
	${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $<

mkent: mkent.o
	${XGCC} -s ${CFLAGS} -I${SRCDIR}/sys -o $@ $< ${MINGW}/lib/libiphlpapi.a

version.o: bsd.rc
	${XWINDRES} -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

bsd.o: bsd.c
	${XGCC} -c ${CFLAGS} -DNOPIE -I${SRCDIR}/sys -o $@ $<

bsd.exe: bsd.o version.o
	${XGCC} -s -static ${CFLAGS} -o $@ $^

libwtrace.o: libwtrace.c
	${XGCC} -c ${CFLAGS} -I${MINGW}/include -I${ROOTDIR}/include -o $@ $<

wtrace: libwtrace.o wtrace.o
	${XGCC} -s ${CFLAGS} -o $@ -L${TRACEDIR} $^ -ltrace \
	${MINGW}/lib/libkernel32.a ${MINGW}/lib/libuser32.a ${MINGW}/lib/libadvapi32.a

mount_refs: ${SBINSRC}/mount_ntfs/mount_ntfs.c ${SBINSRC}/mount/getmntopts.c
	${XGCC} -s ${CFLAGS} -I${SBINSRC}/mount -o $@ $^

install-local: ${BINDIR}

install: ${DESTDIR}
	@mkdir -p ${DESTDIR}${SBINDIR}
	${INSTALL} ${PROGS} ${DESTDIR}${SBINDIR}/
	${INSTALL} -m 02555 -o 0 -g 0 bsd.exe ${DESTDIR}${SBINDIR}/
	${INSTALL} -m 00754 -o 0 -g 0 setup.sh ${DESTDIR}${SBINDIR}/

release-clean:
	rm version.o

release: release-clean all

clean:
	/bin/rm -f *.exe *.o

