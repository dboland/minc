ROOTDIR= ..

include ${ROOTDIR}/Makefile.inc

OBJECTS= crt1.o version.o msvc.o win_posix.o vfs_posix.o libposix.o \
	arch/fork.o arch/syscall.o
LIBPOSIX= libposix-${RELEASE}.dll
IMPLIB= libposix.dll.a
CFLAGS+= -I${ROOTDIR}/include -g
CFLAGS+= -DRELEASE=\"${RELEASE}\" -DVERSION=\"${VERSION}\" -DBUILD=${BUILD}
GCCDIR= ${ROOTDIR}/mingw/gcc
TRACEDIR= ${ROOTDIR}/libtrace
SBINDIR= /sbin
ULIBDIR= /usr/lib

all: ${LIBPOSIX}

${DESTDIR}:
	@echo " ---------------------------------------------------"
	@echo "| MinC install directory ($@) not found. Make sure"
	@echo "| it exists and the 'DESTDIR' variable is defined correctly in config.inc"
	@echo " ---------------------------------------------------"
	@exit 1

${ROOTDIR}/config.inc:
	@echo " ---------------------------------------------------"
	@echo "| Cross-compiler not set up. Please run 'make opt' first."
	@echo " ---------------------------------------------------"
	@exit 1

${INSTALL}:
	@echo " ---------------------------------------------------"
	@echo "| The system must be installed using the system itself."
	@echo "| Switch to the OpenBSD system by running the 'mount' script:"
	@echo "|"
	@echo "| ./mount.sh minc"
	@echo " ---------------------------------------------------"
	@exit 1

crt1.o: crt1.c
	${CC} -c ${CFLAGS} $<

msvc.o: msvc.c
	${CC} -c ${CFLAGS} -o $@ $<

win_posix.o: win/win.c
	${CC} -c ${CFLAGS} -o $@ $<

vfs_posix.o: vfs/vfs.c
	${CC} -c ${CFLAGS} -I${TRACEDIR} -o $@ $<

libposix.o: libposix.c
	${XGCC} -c ${CFLAGS} -I${SRCDIR}/sys -DTTYDEFCHARS -DKTRACE -DSYSVSEM $<

version.o: version.rc
	${XWINDRES} -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

arch/%.o: arch/${PROCESSOR_ARCHITECTURE}/%.S
	${XGCC} -c ${CFLAGS} -o $@ $<

${LIBPOSIX}: ${OBJECTS}
	${LD} -shared -s ${LDFLAGS} -e _DllMainCRTStartup@12 --out-implib=${IMPLIB} \
	-o $@ --image-base=${IMGBASE_LIBPOSIX} ${OBJECTS} \
	-L${MINGW}/lib -L${GCCDIR} -L${TRACEDIR} -lmsvcrt -ladvapi32 -lshell32 -luser32 -lntdll -lkernel32 \
	-lnetapi32 -lws2_32 -liphlpapi -lrpcrt4 -lmpr -lgcc -ltrace

${GLIBDIR}:
	/bin/mkdir -p ${GLIBDIR}

install-local: ${GLIBDIR} ${BINDIR}
	/bin/cp -vu ${LIBPOSIX} ${BINDIR}/
	/bin/cp -vu ${IMPLIB} ${GLIBDIR}/libmingw32.a

install-cross:
	/bin/cp -vu ${IMPLIB} ${XLIBDIR}/libposix.so

install: ${INSTALL} ${DESTDIR}
	@mkdir -p ${DESTDIR}${SBINDIR}/
	${INSTALL} -m 06555 ${LIBPOSIX} ${DESTDIR}${SBINDIR}/
	${INSTALL} ${IMPLIB} ${DESTDIR}${ULIBDIR}/libposix.so

release-clean:
	/bin/rm version.o

release: release-clean all

clean:
	/bin/rm -f arch/*.o *.o *.a *.dll

