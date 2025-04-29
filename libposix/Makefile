include ../Makefile.inc

RELEASE=6.1.0

LIBPOSIX=libposix-${RELEASE}.dll
IMPLIB=libposix.dll.a
CFLAGS+=-I../include -g
CFLAGS+=-DRELEASE=\"${RELEASE}\" -DVERSION=\"${VERSION}\" -DBUILD=${BUILD}
#CFLAGS+=-Wunused
LDFLAGS=-shared -s
OBJECTS=crt1.o win_posix.o msvc_posix.o vfs_posix.o libposix.o
WINDIR=/C/WINDOWS/system32
GCCDIR=../mingw/gcc
TRACEDIR=../libtrace

all: ${LIBPOSIX}

crt1.o: crt1.c
	gcc -c ${CFLAGS} $<

win_posix.o: win/win.c
	gcc -c ${CFLAGS} -I${MINGW}/include -o $@ $<

msvc_posix.o: msvc/msvc.c
	gcc -c ${CFLAGS} -I${MINGW}/include -o $@ $<

vfs_posix.o: vfs/vfs.c
	gcc -c ${CFLAGS} -I${MINGW}/include -I${TRACEDIR} -o $@ $<

libposix.o: libposix.c
	gcc -c ${CFLAGS} -I../openbsd/include -I../openbsd/sys \
	-DTTYDEFCHARS -DKTRACE -DSYSVSEM $<

version.o: version.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

libposix.a: win_posix.o msvc_posix.o
	ar cru $@ $^

arch.d:
	@${MAKE} -C arch CFLAGS="-I../../openbsd/include -I../../include"

${LIBPOSIX}: arch.d ${OBJECTS} version.o
	${LD} ${LDFLAGS} -e _DllMainCRTStartup@12 --out-implib=${IMPLIB} --disable-auto-import --enable-stdcall-fixup \
	-o $@ --image-base=${IMGBASE_LIBPOSIX} ${OBJECTS} $(shell /bin/ls arch/*.o) version.o \
	-L${WINDIR} -L${GCCDIR} -L${TRACEDIR} -lmsvcrt -ladvapi32 -lshell32 -luser32 -lntdll -lkernel32 \
	-lnetapi32 -lws2_32 -liphlpapi -lrpcrt4 -lmpr -lgcc -ltrace

clean:
	/bin/rm -f arch/*.o *.o *.a *.dll

${LIBDIR}:
	/bin/mkdir -p ${LIBDIR}

${BINDIR}:
	/bin/mkdir -p ${BINDIR}

install-local: ${LIBDIR} ${BINDIR}
	/bin/cp ${LIBPOSIX} ${BINDIR}/
	/bin/cp ${IMPLIB} ${LIBDIR}/libmingw32.a

install: ${DESTDIR}
	ginstall -m 06555 ${LIBPOSIX} ${DESTDIR}${SBINDIR}/

