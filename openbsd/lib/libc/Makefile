include ../../../Makefile.inc

RELEASE=75.0

CFLAGS+= -g -Wimplicit-function-declaration -fno-omit-frame-pointer -DNOPIE
#CFLAGS+= -DNLS
CFLAGS+= -ffreestanding
#CFLAGS+= -fno-builtin
LDFLAGS= -shared -s
IMPLIB= libc.dll.a
SRCDIR= /D/openbsd-master/lib/libc
OBJECTS= libc.o thread.o gnu_string.o
SHOBJECTS= $(shell /bin/ls arch/*/*.o */*.o db/*/*.o)
LIBROOT=../../..
GCCLIB=${LIBROOT}/mingw/gcc
TOOLDIR=${LIBROOT}/mingw/bin
POSIXLIB=${LIBROOT}/libposix
WINLIB=/C/WINDOWS/system32
ARCH=i386
LIBC=libc.so.${RELEASE}

NLS=    C.msg Pig.msg da.ISO8859-1.msg da.UTF-8.msg de.ISO8859-1.msg \
        de.UTF-8.msg es.ISO8859-1.msg es.UTF-8.msg fi.ISO8859-1.msg \
        fi.UTF-8.msg fr.ISO8859-1.msg fr.UTF-8.msg it.UTF-8.msg \
        nl.ISO8859-1.msg nl.UTF-8.msg no.ISO8859-1.msg no.UTF-8.msg \
        ru.KOI8-R.msg ru.UTF-8.msg sv.ISO8859-1.msg sv.UTF-8.msg

all: libmsvcrt.a ${LIBC} libc.a

libc.a:
	ar cru $@ ${SHOBJECTS} ${OBJECTS}

libmsvcrt.a: libmsvcrt.def
	dlltool -d $< -l $@ -k

%.d: %
	@CFLAGS="${CFLAGS} -I../include" ${MAKE} -C $< SRCDIR=${SRCDIR} ARCH=${ARCH}

objects: arch.d quad.d stdlib.d citrus.d compat-43.d crypt.d db.d gdtoa.d gen.d \
	hash.d locale.d regex.d stdio.d string.d termios.d time.d thread.d \
	sys.d nls.d dlfcn.d gmon.d

%.o: %.c
	gcc -c ${CFLAGS} -I${LIBROOT}/include -o $@ $<

version.o: version.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

${LIBC}: ${OBJECTS} version.o
	${LD} ${LDFLAGS} -e _DllMainCRTStartup@12 --out-implib=${IMPLIB} --disable-auto-import \
	-o $@ --image-base=${IMGBASE_LIBC} ${GCCLIB}/crt1.o ${OBJECTS} ${SHOBJECTS} version.o \
	-L${GCCLIB} -L${POSIXLIB} -L${WINLIB} -lgcc_eh -lgcc -lposix -lmsvcrt

# --undefined=___progname
# --image-base=${IMGBASE_LIBC}
# --enable-stdcall-fixup

clean:
	/bin/rm -f *.o *.a ${LIBC}

objects-clean:
	/bin/rm -f */*.o arch/*/*.o db/*/*.o

install: ${DESTDIR}
	ginstall ${LIBC} ${DESTDIR}${LIBDIR}/
	@#strip ${DESTDIR}${LIBDIR}/${LIBC}
	ginstall libc.a ${DESTDIR}${LIBDIR}/
	ginstall libmsvcrt.a ${DESTDIR}${LIBDIR}/
	@#ln -s ${LIBC} ${DESTDIR}${LIBDIR}/libc.so
	@#ginstall libc.a C:/minc-1.0${LIBDIR}/

install-local:
	/bin/cp ${IMPLIB} ${LIBDIR}/libc.so
	/bin/cp ${IMPLIB} ${LIBDIR}/libmingwex.dll.a
	/bin/cp ${IMPLIB} ${LIBDIR}/
	/bin/cp libc.a ${LIBDIR}/libmingwex.a
	/bin/cp libc.a ${LIBDIR}/
	/bin/cp libmsvcrt.a ${LIBDIR}/

