include ../../../Makefile.inc

RELEASE=75.0

SRCDIR=/D/openbsd-master/lib
LIBROOT=../../..
POSIXLIB=${LIBROOT}/libposix
GCCLIB=${LIBROOT}/mingw/gcc
TOOLDIR=${LIBROOT}/mingw/bin

CFLAGS= -g -Wimplicit-function-declaration
LDFLAGS= -shared -s
LIBSOCKET= libsocket.so.${RELEASE}
IMPLIB= libsocket.dll.a

all: objects ${LIBSOCKET} libsocket.a

crt1.o: crt1.c
	gcc -c ${CFLAGS} -I../../include -I${LIBROOT}/include $<

libsocket.a:
	ar cru $@ $(shell ls */*.o)

version.o: version.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

${IMPLIB}: libsocket.def
	dlltool -d $< -D ${LIBSOCKET} -l $@

${LIBSOCKET}: crt1.o version.o
	${LD} ${LDFLAGS} -e _DllMainCRTStartup@12 --out-implib=${IMPLIB} --disable-auto-import \
	-o $@ crt1.o --image-base=${IMGBASE_LIBSOCKET} $(shell /bin/ls */*.so) version.o \
	-L${POSIXLIB} -L${GCCLIB} -L../libc -lposix -lc -lmsvcrt -lgcc

%.d: %
	@${MAKE} -C $< SRCDIR="${SRCDIR}" CFLAGS="${CFLAGS} -I../../../../include -DLIBSOCK_IMPORT= -I../../libc/include"

objects: asr.d net.d gen.d rpc.d sys.d

clean: objects-clean
	/bin/rm -f *.o *.a ${LIBSOCKET}

objects-clean:
	/bin/rm -f */*.o */*.so

install: ${DESTDIR}
	ginstall ${LIBSOCKET} ${DESTDIR}${LIBDIR}/
	ginstall libsocket.a ${DESTDIR}${LIBDIR}/
	@#${LN} -s ${LIBSOCKET} ${DESTDIR}${LIBDIR}/libsocket.so

# Perl needs libsocket.so
install-local:
	/bin/cp ${IMPLIB} ${LIBDIR}/
	/bin/cp ${IMPLIB} ${LIBDIR}/libsocket.so
	/bin/cp libsocket.a ${LIBDIR}/
