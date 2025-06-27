include ../../Makefile.inc

PROGS=wrc mkent wtrace mount_refs mknod
TRACEDIR=../../libtrace
SBINPATH= ${SRCDIR}/sbin
LBINDIR= ${PREFIX}/local/bin

all: ${SBINPATH} ${PROGS} bsd.exe

${SBINPATH}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'sbin' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

%.o: %.c
	gcc -c ${CFLAGS} -I../../include -I${SRCDIR}/sys -o $@ $<

%.exe: %.o
	gcc -s ${CFLAGS} -o $@ $<

${LBINDIR}: ${PROGS}
	@/bin/mkdir -p ${LBINDIR}
	/bin/cp -vu mknod mkent ${LBINDIR}/

wrc: wrc.exe

mknod: ${SRCDIR}/sbin/mknod/mknod.c
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
	gcc -c ${CFLAGS} -I${MINGW}/include -I../../include -o $@ $<

wtrace: libwtrace.o wtrace.o
	gcc -s ${CFLAGS} -o $@ -L${TRACEDIR} $^ -ltrace \
	${MINGW}/lib/libkernel32.a ${MINGW}/lib/libuser32.a ${MINGW}/lib/libadvapi32.a

mount_refs:
	@gcc -c ${CFLAGS} -I${SRCDIR}/sbin/mount -o mount_ntfs.o ${SRCDIR}/sbin/mount_ntfs/mount_ntfs.c
	@gcc -c ${CFLAGS} -o getmntopts.o ${SRCDIR}/sbin/mount/getmntopts.c
	gcc -s ${CFLAGS} -o $@ mount_ntfs.o getmntopts.o

install-local: ${LBINDIR}

install: ${DESTDIR}
	@mkdir -p ${DESTDIR}${SBINDIR}
	${INSTALL} ${PROGS} ${DESTDIR}${SBINDIR}/
	${INSTALL} -m 02555 -o 0 -g 0 bsd.exe ${DESTDIR}${SBINDIR}/

clean:
	/bin/rm -f *.exe *.o

