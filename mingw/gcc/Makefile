include ../../Makefile.inc

CFLAGS=-g -Wimplicit-function-declaration
CRT0=crt0.o
OBJECTS=${CRT0} crt1.o crtbegin.o crtend.o gcrt0.o
LIBS=libgcc.a libgcc_eh.a libgcc_s.a
GCCOBJ=_chkstk.o _chkstk_ms.o _mulsc3.o _muldc3.o _mulxc3.o _ctzdi2.o \
	_ffsdi2.o _popcountdi2.o _popcountsi2.o _popcount_tab.o _udivdi3.o
CP=/bin/cp

.DEFAULT:
.SUFFIXES:

all: ${OBJECTS} ${LIBS}

%.o: %.c
	gcc -c ${CFLAGS} -I../../openbsd/include -I../../include -o $@ $<

#
# we need alloca(), which belongs in libgcc.a
# btw: there is a chkstk() in ntdll.dll
#

libgcc.a:
	ar xv ${MINGW}/lib/gcc/mingw32/${GCCVER}/libgcc.a ${GCCOBJ}
	ar cru $@ ${GCCOBJ}

libgcc_eh.a: emutls.o wintls.o gnu_string.o gnu_mntent.o
	ar cru $@ $^

libgcc_s.a: libgcc_s.def
	dlltool -d $< -l $@ -k

# combine objects:

crt0_tls.o: crt0.o win_tls.o
	ld -r -o $@ crt0.o win_tls.o

install-include: ${INCDIR}/c++ ${INCDIR}/ssp
	${CP} -rvu ${MINGW}/lib/gcc/mingw32/${GCCVER}/include/c++ ${INCDIR}/c++/
	${CP} -rvu ${MINGW}/lib/gcc/mingw32/${GCCVER}/include/ssp ${INCDIR}/ssp/

install-local: ${LIBDIR}/gcc/mingw32/${GCCVER}
	${CP} ${LIBS} ${LIBDIR}/gcc/mingw32/${GCCVER}/
	${CP} crtbegin.o crtend.o ${LIBDIR}/gcc/mingw32/${GCCVER}/
	${CP} ${CRT0} ${LIBDIR}/crt2.o
	${CP} crt1.o ${LIBDIR}/dllcrt2.o
	${CP} gcrt0.o ${LIBDIR}/gcrt2.o

install: ${DESTDIR}${LIBDIR}
	cp ${LIBS} ${DESTDIR}${LIBDIR}/gcc/i686-pe-openbsd/${GCCVER}/
	cp crtbegin.o crtend.o ${DESTDIR}${LIBDIR}/
	cp ${CRT0} ${DESTDIR}${LIBDIR}/
	cp crt1.o ${DESTDIR}${LIBDIR}/
	cp gcrt0.o ${DESTDIR}${LIBDIR}/

clean:
	/bin/rm -f *.o *.a
