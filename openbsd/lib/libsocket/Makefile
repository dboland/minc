ROOTDIR=../../..

include ${ROOTDIR}/Makefile.inc

RELEASE=75.0
LIBSOCKET= libsocket.so.${RELEASE}
IMPLIB= libsocket.dll.a
GCCDIR= ${LIBDIR}/gcc/mingw32/${GCCVER}

all: objects lib

lib: ${LIBSOCKET} libsocket.a

crt1.o: crt1.c
	gcc -c -g ${CFLAGS} -I${ROOTDIR}/include $<

libsocket.a:
	@/bin/echo "Making static library $@"
	@ar cru $@ $(shell ls */*.o)

version.o: version.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

${LIBSOCKET}: crt1.o version.o
	${LD} -s -shared ${LDFLAGS} -e _DllMainCRTStartup@12 --out-implib=${IMPLIB} \
	-o $@ crt1.o --image-base=${IMGBASE_LIBSOCKET} $(shell /bin/ls */*.so) version.o \
	-L${LIBDIR} -L${GCCDIR} -lmingw32 -lmingwex -lmsvcrt -lgcc

%.d: %
	@${MAKE} -C $< ROOTDIR=${ROOTDIR}/.. SRCDIR=${SRCDIR} CFLAGS="${CFLAGS} -DLIBSOCK_IMPORT= -I${SRCDIR}/sys -I../../libc/include"

objects: asr.d net.d rpc.d

lib-clean:
	/bin/rm -f *.o *.a ${LIBSOCKET}

objects-clean:
	/bin/rm -f */*.o */*.so

# Perl needs libsocket.so

install-local:
	/bin/cp -vu ${IMPLIB} ${LIBDIR}/
	/bin/cp -vu ${IMPLIB} ${LIBDIR}/libsocket.so
	/bin/cp -vu libsocket.a ${LIBDIR}/

install: ${DESTDIR}
	${INSTALL} ${LIBSOCKET} libsocket.a ${DESTDIR}/usr/lib/
	@#${LN} -s ${LIBSOCKET} ${DESTDIR}/usr/lib/libsocket.so

clean: objects-clean lib-clean

