include ../../Makefile.inc

CRT0= crt0.o
OBJECTS= ${CRT0} crt1.o crtbegin.o crtend.o
LIBS= libgcc.a libgcc_s.a libgcc_eh.a
GCCOBJ= _mulsc3.o _muldc3.o _mulxc3.o _ctzdi2.o \
	_ffsdi2.o _popcountdi2.o _popcountsi2.o _popcount_tab.o _udivdi3.o \
	_divdi3.o _clrsbdi2.o _clrsbsi2.o \
	_chkstk.o _chkstk_ms.o 
GCCDIR= /lib/gcc/mingw32/${GCCVER}
ULIBDIR= /usr/lib

all: ${MINGW}${GCCDIR} ${OBJECTS} ${LIBS}

${MINGW}${GCCDIR}:
	@echo " -----------------------------------------------"
	@echo "| GCC libexec directory ($@) not found. Make sure"
	@echo "| the 'GCCVER' variable is defined correctly in Makefile.inc"
	@echo "| Run 'gcc -v' to find out the right version number.
	@echo " -----------------------------------------------"
	@exit 1

%.o: %.c
	gcc -c ${CFLAGS} -I../../include -o $@ $<

#
# we need alloca(), which belongs in libgcc.a
# btw: there is a chkstk() in ntdll.dll
#

#second stage:
#gnu_mntent.o  futimesat.o

libgcc.a: wintls.o emutls.o mempcpy.o
	ar xv ${MINGW}${GCCDIR}/libgcc.a ${GCCOBJ}
	ar cru $@ ${GCCOBJ} $^

libgcc_s.a: libgcc_s.def
	dlltool -d $< -l $@ -k

libgcc_eh.a: libgcc_eh.def
	dlltool -d $< -l $@ -k

#libgcc_eh.a: emutls.o wintls.o mempcpy.o gnu_mntent.o futimesat.o
#	ar cru $@ $^

# use -r switch to combine objects:

crt0_tls.o: crt0.o win_tls.o
	ld -r -o $@ crt0.o win_tls.o

${PREFIX}${GCCDIR}: ${MINGW}${GCCDIR}
	/bin/mkdir -p ${PREFIX}${GCCDIR}
	/bin/cp -vu ${MINGW}${GCCDIR}/libstdc++.dll.a ${PREFIX}${GCCDIR}/libstdc++.a

GCCINC=${PREFIX}${GCCDIR}/include

includes-local: ${GCCINC}
	/bin/cp -rvu ${MINGW}${GCCINC}/c++ ${GCCINC}/
	/bin/cp -rvu ${MINGW}${GCCINC}/ssp ${GCCINC}/

install-local: ${PREFIX}${GCCDIR}
	/bin/cp -vu ${LIBS} ${PREFIX}${GCCDIR}/
	/bin/cp -vu crtbegin.o crtend.o ${PREFIX}${GCCDIR}/
	/bin/cp -vu ${CRT0} ${LIBDIR}/crt2.o
	/bin/cp -vu crt1.o ${LIBDIR}/dllcrt2.o
	@#/bin/cp -vu gcrt0.o ${LIBDIR}/gcrt2.o

install: ${DESTDIR}${ULIBDIR}
	${INSTALL} libgcc.a ${DESTDIR}${ULIBDIR}/gcc/i686-pe-openbsd/${GCCVER}/
	${INSTALL} crtbegin.o crtend.o ${DESTDIR}${ULIBDIR}/
	${INSTALL} ${CRT0} ${DESTDIR}${ULIBDIR}/
	${INSTALL} crt1.o ${DESTDIR}${ULIBDIR}/
	${INSTALL} gcrt0.o ${DESTDIR}${ULIBDIR}/

clean:
	/bin/rm -f *.o *.a
