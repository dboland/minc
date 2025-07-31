ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

RELEASE= 75.0
OBJECTS= libc.o
SHOBJECTS= $(shell /bin/ls arch/${ARCH}/*/*.o */*.o db/*/*.o)
LIBC= libc.so.${RELEASE}
GCCDIR= ${GLIBDIR}/gcc/mingw32/${GCCVER}
CFLAGS+= -g -DNOPIE
IMPLIB= libc.dll.a

NLS=    C.msg Pig.msg da.ISO8859-1.msg da.UTF-8.msg de.ISO8859-1.msg \
        de.UTF-8.msg es.ISO8859-1.msg es.UTF-8.msg fi.ISO8859-1.msg \
        fi.UTF-8.msg fr.ISO8859-1.msg fr.UTF-8.msg it.UTF-8.msg \
        nl.ISO8859-1.msg nl.UTF-8.msg no.ISO8859-1.msg no.UTF-8.msg \
        ru.KOI8-R.msg ru.UTF-8.msg sv.ISO8859-1.msg sv.UTF-8.msg

all: include lib

%.d: %
	@CFLAGS="${CFLAGS}" ${MAKE} -C $< XGCC="${XGCC}" ROOTDIR="${ROOTDIR}/.." \
	SRCDIR=${SRCDIR} ARCH=${ARCH}

%.o: %.c
	${XGCC} -c ${CFLAGS} -I${ROOTDIR}/include -o $@ $<

libc.a:
	@/bin/echo "Making static library $@"
	@ar cru $@ ${OBJECTS} ${SHOBJECTS}

version.o: version.rc
	${XWINDRES} -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

${LIBC}: objects ${OBJECTS} version.o
	${LD} -shared ${LDFLAGS} -e _DllMainCRTStartup@12 --out-implib=${IMPLIB} \
	-o $@ --image-base=${IMGBASE_LIBC} ${GLIBDIR}/dllcrt2.o ${OBJECTS} ${SHOBJECTS} version.o \
	-L${GLIBDIR} -L${GCCDIR} -lmingw32 -lmsvcrt -lgcc

include:
	/bin/cp -rvu ${SRCDIR}/lib/libc/include .
	/bin/patch -p 2 -d . <../../libc.patch

objects: arch.d quad.d stdlib.d citrus.d compat-43.d crypt.d db.d gdtoa.d gen.d \
	hash.d locale.d regex.d stdio.d string.d termios.d time.d thread.d \
	sys.d nls.d dlfcn.d gmon.d net.d

lib: ${LIBC} libc.a

lib-clean:
	/bin/rm -f *.o *.a ${LIBC}

objects-clean:
	/bin/rm -f */*.o arch/${ARCH}/*/*.o db/*/*.o

release-clean:
	/bin/rm -f version.o

# Perl needs libc.so

install-local:
	/bin/cp -vu ${IMPLIB} ${GLIBDIR}/libc.so
	/bin/cp -vu ${IMPLIB} ${GLIBDIR}/libmingwex.dll.a
	/bin/cp -vu ${IMPLIB} ${GLIBDIR}/
	/bin/cp -vu libc.a ${GLIBDIR}/libmingwex.a
	/bin/cp -vu libc.a ${GLIBDIR}/

install-cross:
	/bin/cp -vu libc.a ${XLIBDIR}/
	/bin/cp -vu ${IMPLIB} ${XLIBDIR}/libc.so

install: ${DESTDIR}
	@mkdir -p ${DESTDIR}/usr/lib
	${INSTALL} ${LIBC} libc.a ${DESTDIR}/usr/lib/
	@#ln -s ${LIBC} ${DESTDIR}/usr/lib/libc.so

clean: objects-clean lib-clean

