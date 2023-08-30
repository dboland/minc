include ../Makefile.inc

RELEASE=6.1.0
BINDIR=/bin

LIBPOSIX=libposix-${RELEASE}.dll
IMPLIB=libposix.dll.a
CFLAGS+=-g -Wimplicit-function-declaration -I../include -ffreestanding
CFLAGS+=-DRELEASE=\"${RELEASE}\" -DVERSION=\"${VERSION}\" -DBUILD=${BUILD}
#CFLAGS+=-Wunused
LDFLAGS=-s
OBJECTS=crt1.o win_posix.o msvc_posix.o vfs_posix.o libposix.o
WINDIR=/C/WINDOWS/system32
GCCDIR=../mingw/gcc
INSTALL=../mingw/bin/ginstall

all: ${LIBPOSIX}

crt1.o: crt1.c
	gcc -c ${CFLAGS} $<

win_posix.o: win/win.c
	gcc -c ${CFLAGS} -I${MINGW}/include -o $@ $<

msvc_posix.o: msvc/msvc.c
	gcc -c ${CFLAGS} -I${MINGW}/include -o $@ $<

vfs_posix.o: vfs/vfs.c
	gcc -c ${CFLAGS} -I${MINGW}/include -o $@ $<

libposix.o: libposix.c
	gcc -c ${CFLAGS} -DTTYDEFCHARS -I../openbsd/include -I../openbsd/sys \
	-DLIBPOSIX_IMPORT= -DKTRACE -DSYSVSEM $<

version.o: version.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

arch.d:
	@${MAKE} -C arch CFLAGS="-I../../openbsd/include -I../../include"

${LIBPOSIX}: arch.d ${OBJECTS} version.o
	${LD} -shared ${LDFLAGS} -e _DllMainCRTStartup@12 --out-implib=${IMPLIB} --disable-auto-import --enable-stdcall-fixup \
	-o $@ --image-base=${IMGBASE_LIBPOSIX} ${OBJECTS} $(shell /bin/ls arch/*.o) version.o \
	-L${WINDIR} -L${GCCDIR} -lmsvcrt -ladvapi32 -lshell32 -luser32 -lntdll -lkernel32 \
	-lnetapi32 -lws2_32 -limagehlp -liphlpapi -lrpcrt4 -lgcc

clean:
	/bin/rm -f arch/*.o *.o *.a *.dll

${LIBDIR}:
	mkdir -p ${DESTDIR}${LIBDIR}

${BINDIR}:
	mkdir -p ${DESTDIR}${BINDIR}

install: ${DESTDIR}
	${INSTALL} -m 06555 ${LIBPOSIX} ${DESTDIR}${BINDIR}/

install-local: ${LIBDIR} ${BINDIR}
	/bin/cp ${LIBPOSIX} ${BINDIR}/
	/bin/cp ${IMPLIB} ${LIBDIR}/libmingw32.a

