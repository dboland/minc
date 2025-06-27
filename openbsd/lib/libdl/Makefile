ROOTDIR=../../..

include ${ROOTDIR}/Makefile.inc

LIBEXEC=${PREFIX}/libexec
OBJECTS=libdl.o
IMPLIB=libdl.dll.a
LDFLAGS=-shared -s

all: ld.so

%.o: %.c
	gcc -c -g ${CFLAGS} -I${ROOTDIR}/include -I${SRCDIR}/sys $<

version.o: version.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

ld.so: ${OBJECTS} version.o
	${LD} ${LDFLAGS} -e _DllMainCRTStartup@12 --out-implib=${IMPLIB} \
	-o $@ --image-base=${IMGBASE_LIBD} ${LIBDIR}/dllcrt2.o ${OBJECTS} version.o \
	-L${LIBDIR} -lmingw32

clean:
	/bin/rm -f *.o *.a ld.so

${LIBEXEC}:
	/bin/mkdir ${LIBEXEC}

install-local: ${LIBEXEC}
	/bin/cp -vu ld.so ${LIBEXEC}/
	/bin/cp -vu ${IMPLIB} ${LIBDIR}/

install: ${DESTDIR}
	@mkdir -p ${DESTDIR}${LIBEXEC}
	${INSTALL} ld.so ${DESTDIR}${LIBEXEC}/
	${INSTALL} ${IMPLIB} ${DESTDIR}${LIBDIR}/libdl.a

