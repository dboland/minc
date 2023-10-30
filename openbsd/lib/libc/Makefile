include ../../../Makefile.inc

RELEASE=75.0

ARCH=i386
CFLAGS+= -g -ffreestanding -DNOPIE
#CFLAGS+= -fno-builtin
LDFLAGS= -shared -s
IMPLIB= libc.dll.a
SRCDIR= /D/openbsd-master/lib/libc
OBJECTS= libc.o
SHOBJECTS= $(shell /bin/ls arch/${ARCH}/*/*.o */*.o db/*/*.o)
LIBROOT=../../..
GCCLIB=${LIBROOT}/mingw/gcc
TOOLDIR=${LIBROOT}/mingw/bin
POSIXLIB=${LIBROOT}/libposix
WINLIB=/C/WINDOWS/system32
LIBC=libc.so.${RELEASE}

NLS=    C.msg Pig.msg da.ISO8859-1.msg da.UTF-8.msg de.ISO8859-1.msg \
        de.UTF-8.msg es.ISO8859-1.msg es.UTF-8.msg fi.ISO8859-1.msg \
        fi.UTF-8.msg fr.ISO8859-1.msg fr.UTF-8.msg it.UTF-8.msg \
        nl.ISO8859-1.msg nl.UTF-8.msg no.ISO8859-1.msg no.UTF-8.msg \
        ru.KOI8-R.msg ru.UTF-8.msg sv.ISO8859-1.msg sv.UTF-8.msg

all: objects lib

lib: ${LIBC} libc.a libmsvcrt.a

libc.a:
	ar cru $@ ${OBJECTS} ${SHOBJECTS}

libmsvcrt.a: libmsvcrt.def
	dlltool -d $< -l $@ -k

%.d: %
	@CFLAGS="${CFLAGS} -I../include" ${MAKE} -C $< SRCDIR=${SRCDIR} ARCH=${ARCH}

objects: arch.d quad.d stdlib.d citrus.d compat-43.d crypt.d db.d gdtoa.d gen.d \
	hash.d locale.d regex.d stdio.d string.d termios.d time.d thread.d \
	sys.d nls.d dlfcn.d gmon.d net.d

%.o: %.c
	gcc -c ${CFLAGS} -I${LIBROOT}/include -o $@ $<

version.o: version.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

${LIBC}: ${OBJECTS} version.o
	${LD} ${LDFLAGS} -e _DllMainCRTStartup@12 --out-implib=${IMPLIB} --disable-auto-import \
	-o $@ --image-base=${IMGBASE_LIBC} ${GCCLIB}/crt1.o ${OBJECTS} ${SHOBJECTS} version.o \
	-L${GCCLIB} -L${POSIXLIB} -L${WINLIB} -lgcc -lposix -lmsvcrt

clean: objects-clean lib-clean

lib-clean:
	/bin/rm -f *.o *.a ${LIBC}

objects-clean:
	/bin/rm -f */*.o arch/${ARCH}/*/*.o db/*/*.o

# Perl needs libc.so
install-local:
	/bin/cp ${IMPLIB} ${LIBDIR}/libc.so
	/bin/cp ${IMPLIB} ${LIBDIR}/libmingwex.dll.a
	/bin/cp ${IMPLIB} ${LIBDIR}/
	/bin/cp libc.a ${LIBDIR}/libmingwex.a
	/bin/cp libc.a ${LIBDIR}/
	/bin/cp libmsvcrt.a ${LIBDIR}/

install: ${DESTDIR}
	ginstall ${LIBC} ${DESTDIR}${LIBDIR}/
	ginstall libc.a ${DESTDIR}${LIBDIR}/
	ginstall libmsvcrt.a ${DESTDIR}${LIBDIR}/
	@#ln -s ${LIBC} ${DESTDIR}${LIBDIR}/libc.so

