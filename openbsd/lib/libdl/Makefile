include ../../../Makefile.inc

RELEASE=6.1.0

LIBEXEC=${PREFIX}/libexec

CFLAGS+=-g
OBJECTS=libdl.o
IMPLIB=libdl.dll.a
LDFLAGS=-shared -s
LIBROOT=../../..
POSIXDIR=${LIBROOT}/libposix
GCCLIB=${LIBROOT}/mingw/gcc

all: ld.so

%.o: %.c
	gcc -c ${CFLAGS} -I${LIBROOT}/include -I../../include $<

version.o: version.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

ld.so: ${OBJECTS} version.o
	${LD} ${LDFLAGS} -e _DllMainCRTStartup@12 --out-implib=${IMPLIB} --disable-auto-import --enable-stdcall-fixup \
	-o $@ --image-base=${IMGBASE_LIBD} ${GCCLIB}/crt1.o ${OBJECTS} version.o \
	-L${POSIXDIR} -lposix

clean:
	/bin/rm -f *.o *.a ld.so

${LIBEXEC}:
	/bin/mkdir ${LIBEXEC}

install-local: ${LIBEXEC}
	/bin/cp ld.so ${LIBEXEC}/
	/bin/cp ${IMPLIB} ${LIBDIR}/

install: ${DESTDIR}
	ginstall ld.so ${DESTDIR}${LIBEXEC}/
	ginstall ${IMPLIB} ${DESTDIR}${LIBDIR}/libdl.a

