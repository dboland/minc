ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

LIBEXEC= /usr/libexec
OBJECTS= libdl.o
IMPLIB= libdl.dll.a
LDFLAGS= -shared -s
LIBDL= ld.so

all: lib

%.o: %.c
	gcc -c -g ${CFLAGS} -I${ROOTDIR}/include -I${SRCDIR}/sys $<

version.o: version.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

${LIBDL}: ${OBJECTS} version.o
	${LD} ${LDFLAGS} -e _DllMainCRTStartup@12 --out-implib=${IMPLIB} \
	-o $@ --image-base=${IMGBASE_LIBD} ${LIBDIR}/dllcrt2.o ${OBJECTS} version.o \
	-L${LIBDIR} -lmingw32

lib: ${LIBDL}

${LIBEXEC}:
	/bin/mkdir ${LIBEXEC}

install-local: ${LIBEXEC}
	/bin/cp -vu ld.so ${LIBEXEC}/
	/bin/cp -vu ${IMPLIB} ${LIBDIR}/

install-cross:
	/bin/cp -vu ${IMPLIB} ${XLIBDIR}/libdl.a

install: ${DESTDIR}
	@mkdir -p ${DESTDIR}${LIBEXEC}
	${INSTALL} ld.so ${DESTDIR}${LIBEXEC}/
	${INSTALL} ${IMPLIB} ${DESTDIR}/usr/lib/libdl.a

release-clean:
	/bin/rm -f version.o

clean:
	/bin/rm -f *.o *.a ${LIBDL}

