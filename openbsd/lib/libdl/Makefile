include ../../../Makefile.inc

LIBEXEC=${PREFIX}/libexec

OBJECTS=libdl.o
IMPLIB=libdl.dll.a
LDFLAGS=-shared -s
LIBROOT=../../..
POSIXDIR=${LIBROOT}/libposix
GCCLIB=${LIBROOT}/mingw/gcc

all: ld.so

%.o: %.c
	gcc -c -g ${CFLAGS} -I${LIBROOT}/include -I${SRCDIR}/sys $<

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
	/bin/cp -vu ld.so ${LIBEXEC}/
	/bin/cp -vu ${IMPLIB} ${LIBDIR}/

install: ${DESTDIR}
	install ld.so ${DESTDIR}${LIBEXEC}/
	install ${IMPLIB} ${DESTDIR}${LIBDIR}/libdl.a

