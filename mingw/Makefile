include ../Makefile.inc

LIBEXEC=${PREFIX}/libexec
LIBS=libadvapi32.a libkernel32.a libshell32.a libuser32.a \
	libmoldname.a libgmon.a libmsvcrt.a

all: ${MINGW} ${LIBS} libiphlpapi.dll.a
	@${MAKE} -C gcc

${MINGW}:
	@echo " -----------------------------------------------"
	@echo "| MinGW root directory ($@) not found. Make sure"
	@echo "| the 'MINGW' variable is defined correctly in Makefile.inc"
	@echo " -----------------------------------------------"
	@exit 1

#
# below -k option makes ld remove "@" suffix
#

%.a: %.def
	dlltool -d $< -l $@ -k

${UBINDIR}/%.exe: ${MINGW}/bin/%.exe
	@/bin/cp -vu $^ $@

${BINDIR}/%.dll: ${MINGW}/bin/%.dll
	@/bin/cp -vu $^ $@

${UBINDIR}:
	@/bin/mkdir -p ${UBINDIR}

${LIBDIR}:
	@/bin/mkdir -p ${LIBDIR}

${LIBEXEC}:
	@/bin/mkdir -p ${LIBEXEC}
	/bin/cp -rvu ${MINGW}/libexec/gcc ${LIBEXEC}/

GCCBIN+= ${UBINDIR}/gcc.exe ${UBINDIR}/cpp.exe ${UBINDIR}/gdb.exe
GCCLIB+= ${BINDIR}/libgmp-10.dll ${BINDIR}/libmpc-3.dll ${BINDIR}/libmpfr-4.dll \
	${BINDIR}/zlib1.dll ${BINDIR}/libgcc_s_dw2-1.dll
GCCBIN+= ${UBINDIR}/g++.exe ${UBINDIR}/c++.exe
GCCLIB+= ${BINDIR}/libintl-8.dll ${BINDIR}/libiconv-2.dll ${BINDIR}/libstdc++-6.dll

BINUTILS= ${UBINDIR}/ld.exe ${UBINDIR}/ar.exe ${UBINDIR}/as.exe ${UBINDIR}/nm.exe \
	${UBINDIR}/objdump.exe ${UBINDIR}/ranlib.exe ${UBINDIR}/size.exe \
	${UBINDIR}/strip.exe ${UBINDIR}/dlltool.exe ${UBINDIR}/gprof.exe \
	${UBINDIR}/addr2line.exe ${UBINDIR}/dllwrap.exe ${UBINDIR}/elfedit.exe \
	${UBINDIR}/objcopy.exe ${UBINDIR}/strings.exe ${UBINDIR}/windmc.exe \
	${UBINDIR}/windres.exe ${UBINDIR}/c++filt.exe

MSGBIN= ${UBINDIR}/msgfmt.exe ${BINDIR}/libgettextsrc-0-18-3.dll \
	${BINDIR}/libgettextlib-0-18-3.dll

install-gcc: ${BINDIR} ${UBINDIR} ${GCCBIN} ${GCCLIB}
install-ld: ${UBINDIR} ${BINUTILS}
install-tools: ${MSGBIN}

# GetIpStatisticsEx() is missing in vanilla MinGW (libiphlpapi.dll.a)
# tip: create dummy /libexec/ld.so for configure scripts to detect shared libs

install-local: ${MINGW} install-gcc install-ld ${LIBEXEC} ${LIBDIR}
	/bin/cp -vu ${LIBS} ${LIBDIR}/
	/bin/cp -vu libiphlpapi.dll.a ${MINGW}/lib/
	@${MAKE} -C gcc install-local

install: ${DESTDIR}
	${INSTALL} libmsvcrt.a ${DESTDIR}${LIBDIR}/
	@${MAKE} -C gcc install

clean:
	/bin/rm -f *.a
	@${MAKE} -C gcc clean
