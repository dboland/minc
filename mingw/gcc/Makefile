include ../../Makefile.inc

CRT0=crt0.o
OBJECTS=${CRT0} crt1.o crtbegin.o crtend.o gcrt0.o
LIBS=libgcc.a libgcc_s.a libgcc_eh.a
GCCOBJ=_mulsc3.o _muldc3.o _mulxc3.o _ctzdi2.o \
	_ffsdi2.o _popcountdi2.o _popcountsi2.o _popcount_tab.o _udivdi3.o \
	_divdi3.o _clrsbdi2.o _clrsbsi2.o \
	_chkstk.o _chkstk_ms.o 
GCCINC=/lib/gcc/mingw32/${GCCVER}/include

all: ${OBJECTS} ${LIBS}

%.o: %.c
	gcc -c ${CFLAGS} -I../../openbsd/include -I../../include -o $@ $<

#
# we need alloca(), which belongs in libgcc.a
# btw: there is a chkstk() in ntdll.dll
#

libgcc.a: wintls.o emutls.o mempcpy.o gnu_mntent.o futimesat.o
	ar xv ${MINGW}/lib/gcc/mingw32/${GCCVER}/libgcc.a ${GCCOBJ}
	ar cru $@ ${GCCOBJ} $^
# wintls.o

libgcc_s.a: libgcc_s.def
	dlltool -d $< -l $@ -k

libgcc_eh.a: libgcc_eh.def
	dlltool -d $< -l $@ -k

#libgcc_eh.a: emutls.o wintls.o mempcpy.o gnu_mntent.o futimesat.o
#	ar cru $@ $^

# combine objects:

crt0_tls.o: crt0.o win_tls.o
	ld -r -o $@ crt0.o win_tls.o

includes-local: ${CPPINC}
	/bin/cp -rvu ${MINGW}${CPPINC}/c++ ${GCCINC}/
	/bin/cp -rvu ${MINGW}${CPPINC}/ssp ${GCCINC}/

install-local: ${LIBDIR}/gcc/mingw32/${GCCVER}
	/bin/cp ${MINGW}/lib/gcc/mingw32/${GCCVER}/libstdc++.dll.a ${LIBDIR}/gcc/mingw32/${GCCVER}/libstdc++.a
	/bin/cp ${LIBS} ${LIBDIR}/gcc/mingw32/${GCCVER}/
	/bin/cp crtbegin.o crtend.o ${LIBDIR}/gcc/mingw32/${GCCVER}/
	/bin/cp ${CRT0} ${LIBDIR}/crt2.o
	/bin/cp crt1.o ${LIBDIR}/dllcrt2.o
	/bin/cp gcrt0.o ${LIBDIR}/gcrt2.o

install: ${DESTDIR}${LIBDIR}
	cp libgcc.a ${DESTDIR}${LIBDIR}/gcc/i686-pe-openbsd/${GCCVER}/
	cp crtbegin.o crtend.o ${DESTDIR}${LIBDIR}/
	cp ${CRT0} ${DESTDIR}${LIBDIR}/
	cp crt1.o ${DESTDIR}${LIBDIR}/
	cp gcrt0.o ${DESTDIR}${LIBDIR}/

clean:
	/bin/rm -f *.o *.a
