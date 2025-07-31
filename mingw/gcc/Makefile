ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

OBJECTS= crt0.o crt1.o crtbegin.o crtend.o
LIBS= libgcc.a libgcc_s.a libgcc_eh.a
GCCOBJ= _mulsc3.o _muldc3.o _mulxc3.o _ctzdi2.o \
	_divdi3.o _udivdi3.o _clrsbdi2.o _clrsbsi2.o \
	_chkstk.o _chkstk_ms.o
GCCDIR= /lib/gcc/mingw32/${GCCVER}
ULIBDIR= /usr/lib

all: ${MINGW}${GCCDIR} ${OBJECTS} ${LIBS}

${MINGW}${GCCDIR}:
	@echo " -----------------------------------------------"
	@echo "| GCC libexec directory ($@) not found. Make sure"
	@echo "| the 'GCCVER' variable is defined correctly in config.inc"
	@echo "| Run 'gcc -v' to find out the right version number."
	@echo " -----------------------------------------------"
	@exit 1

%.o: %.c
	gcc -c ${CFLAGS} -I${ROOTDIR}/include -o $@ $<

%.a: %.def
	dlltool -d $< -l $@ -k

#
# we need alloca(), which belongs in libgcc.a
# btw: there is a chkstk() in ntdll.dll
#

#second stage:
#gnu_mntent.o  futimesat.o mempcpy.o

libgcc.a: wintls.o emutls.o
	ar xv ${MINGW}${GCCDIR}/libgcc.a ${GCCOBJ}
	ar cru $@ ${GCCOBJ} $^

#libgcc_s.a: wintls.o emutls.o
#	ar cru $@ $^

# use -r switch to combine objects:

crt0_tls.o: crt0.o win_tls.o
	ld -r -o $@ crt0.o win_tls.o

#${GPREFIX}${GCCDIR}/include:
#	@/bin/mkdir -p $@
#	/bin/cp -ru ${MINGW}${GCCDIR}/include/c++ $@/
#	/bin/cp -ru ${MINGW}${GCCDIR}/include/ssp $@/

#${GPREFIX}${GCCDIR}: ${MINGW}${GCCDIR}
#	@/bin/mkdir -p $@
#	/bin/cp -vu ${MINGW}${GCCDIR}/libgcc.a ${GPREFIX}${GCCDIR}/
#	/bin/cp -vu ${MINGW}${GCCDIR}/libssp.a ${GPREFIX}${GCCDIR}/
#	/bin/cp -vu ${MINGW}${GCCDIR}/libstdc++.a ${GPREFIX}${GCCDIR}/

install-local:
	@/bin/mkdir -p ${GPREFIX}${GCCDIR}
	/bin/cp -vu ${LIBS} ${GPREFIX}${GCCDIR}/
	/bin/cp -vu crtbegin.o crtend.o ${GPREFIX}${GCCDIR}/
	/bin/cp -vu crt0.o ${GLIBDIR}/crt2.o
	/bin/cp -vu crt1.o ${GLIBDIR}/dllcrt2.o
	@#/bin/cp -vu gcrt0.o ${GLIBDIR}/gcrt2.o

install-cross:
	/bin/cp -vu crtbegin.o crtend.o ${XLIBDIR}/
	/bin/cp -vu crt0.o crt1.o ${XLIBDIR}/

install: ${DESTDIR}${ULIBDIR}
	@#${INSTALL} libgcc.a ${DESTDIR}${ULIBDIR}/gcc/i686-pe-openbsd/${GCCVER}/
	${INSTALL} crtbegin.o crtend.o ${DESTDIR}${ULIBDIR}/
	${INSTALL} crt0.o crt1.o ${DESTDIR}${ULIBDIR}/
	${INSTALL} gcrt0.o ${DESTDIR}${ULIBDIR}/

clean:
	/bin/rm -f *.o *.a
