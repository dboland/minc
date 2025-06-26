include ../../../Makefile.inc

RELEASE=75.0

ARCH=i386
CFLAGS+= -g -DNOPIE
LDFLAGS+= -shared -s
IMPLIB= libc.dll.a
OBJECTS= libc.o
SHOBJECTS= $(shell /bin/ls arch/${ARCH}/*/*.o */*.o db/*/*.o)
LIBROOT=../../..
LIBC= libc.so.${RELEASE}
GCCLIB= ${LIBDIR}/gcc/mingw32/${GCCVER}

NLS=    C.msg Pig.msg da.ISO8859-1.msg da.UTF-8.msg de.ISO8859-1.msg \
        de.UTF-8.msg es.ISO8859-1.msg es.UTF-8.msg fi.ISO8859-1.msg \
        fi.UTF-8.msg fr.ISO8859-1.msg fr.UTF-8.msg it.UTF-8.msg \
        nl.ISO8859-1.msg nl.UTF-8.msg no.ISO8859-1.msg no.UTF-8.msg \
        ru.KOI8-R.msg ru.UTF-8.msg sv.ISO8859-1.msg sv.UTF-8.msg

all: objects lib

%.d: %
	@CFLAGS="${CFLAGS}" ${MAKE} -C $< SRCDIR=${SRCDIR} ARCH=${ARCH}

%.o: %.c
	gcc -c ${CFLAGS} -I${LIBROOT}/include -o $@ $<

libc.a:
	ar cru $@ ${OBJECTS} ${SHOBJECTS}

version.o: version.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

${LIBC}: ${OBJECTS} version.o
	${LD} ${LDFLAGS} -e _DllMainCRTStartup@12 --out-implib=${IMPLIB} \
	-o $@ --image-base=${IMGBASE_LIBC} /usr/lib/dllcrt2.o ${OBJECTS} ${SHOBJECTS} version.o \
	-L/usr/lib -L${GCCLIB} -lmingw32 -lmsvcrt -lgcc

objects: arch.d quad.d stdlib.d citrus.d compat-43.d crypt.d db.d gdtoa.d gen.d \
	hash.d locale.d regex.d stdio.d string.d termios.d time.d thread.d \
	sys.d nls.d dlfcn.d gmon.d net.d

lib: ${LIBC} libc.a

clean: objects-clean lib-clean

lib-clean:
	/bin/rm -f *.o *.a ${LIBC}

objects-clean:
	/bin/rm -f */*.o arch/${ARCH}/*/*.o db/*/*.o

# Perl needs libc.so
install-local:
	/bin/cp -vu ${IMPLIB} ${LIBDIR}/libc.so
	/bin/cp -vu ${IMPLIB} ${LIBDIR}/libmingwex.dll.a
	/bin/cp -vu ${IMPLIB} ${LIBDIR}/
	/bin/cp -vu libc.a ${LIBDIR}/libmingwex.a
	/bin/cp -vu libc.a ${LIBDIR}/
	/bin/cp -vu libmsvcrt.a ${LIBDIR}/

install: ${DESTDIR}
	${INSTALL} ${LIBC} ${DESTDIR}${LIBDIR}/
	${INSTALL} libc.a ${DESTDIR}${LIBDIR}/
	@#ln -s ${LIBC} ${DESTDIR}${LIBDIR}/libc.so

