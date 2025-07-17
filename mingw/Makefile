include ../Makefile.inc

LIBEXEC=${PREFIX}/libexec
LIBS=libadvapi32.a libkernel32.a libshell32.a libuser32.a \
	libmoldname.a libgmon.a libmsvcrt.a
ULIBDIR= /usr/lib

all: ${MINGW} ${LIBS} libiphlpapi.dll.a
	@${MAKE} -C gcc

${MINGW}:
	@echo " -----------------------------------------------"
	@echo "| MinGW root directory ($@) not found. Make sure"
	@echo "| the 'MINGW' variable is defined correctly in config.inc"
	@echo " -----------------------------------------------"
	@exit 1

#
# below -k option makes ld remove "@" suffix
#

%.a: %.def
	dlltool -d $< -l $@ -k

${BINDIR}/%.exe: ${MINGW}/bin/%.exe
	@/bin/cp -vu $^ $@

${BINDIR}/%.dll: ${MINGW}/bin/%.dll
	@/bin/cp -vu $^ $@

${BINDIR}:
	@/bin/mkdir -p ${BINDIR}

${LIBDIR}:
	@/bin/mkdir -p ${LIBDIR}

${LIBEXEC}:
	@/bin/mkdir -p ${LIBEXEC}
	/bin/cp -ru ${MINGW}/libexec/gcc ${LIBEXEC}/

GCCBIN+= ${BINDIR}/gcc.exe ${BINDIR}/cpp.exe ${BINDIR}/gdb.exe
GCCLIB+= ${BINDIR}/libgmp-10.dll ${BINDIR}/libmpc-3.dll ${BINDIR}/libmpfr-4.dll \
	${BINDIR}/zlib1.dll ${BINDIR}/libgcc_s_dw2-1.dll
GCCBIN+= ${BINDIR}/g++.exe ${BINDIR}/c++.exe
GCCLIB+= ${BINDIR}/libintl-8.dll ${BINDIR}/libiconv-2.dll ${BINDIR}/libstdc++-6.dll

BINUTILS= ${BINDIR}/ld.exe ${BINDIR}/ar.exe ${BINDIR}/as.exe ${BINDIR}/nm.exe \
	${BINDIR}/objdump.exe ${BINDIR}/ranlib.exe ${BINDIR}/size.exe \
	${BINDIR}/strip.exe ${BINDIR}/dlltool.exe ${BINDIR}/gprof.exe \
	${BINDIR}/addr2line.exe ${BINDIR}/dllwrap.exe ${BINDIR}/elfedit.exe \
	${BINDIR}/objcopy.exe ${BINDIR}/strings.exe ${BINDIR}/windmc.exe \
	${BINDIR}/windres.exe ${BINDIR}/c++filt.exe

MSGBIN= ${BINDIR}/msgfmt.exe ${BINDIR}/libgettextsrc-0-18-3.dll \
	${BINDIR}/libgettextlib-0-18-3.dll

install-gcc: ${BINDIR} ${GCCBIN} ${GCCLIB}
install-ld: ${BINDIR} ${BINUTILS}
install-tools: ${MSGBIN}

# GetIpStatisticsEx() is missing in vanilla MinGW (libiphlpapi.dll.a)
# tip: create dummy /libexec/ld.so for configure scripts to detect shared libs

install-local: ${MINGW} install-gcc install-ld ${LIBEXEC} ${LIBDIR}
	/bin/cp -vu ${LIBS} ${LIBDIR}/
	/bin/cp -vu libiphlpapi.dll.a ${MINGW}/lib/
	@${MAKE} -C gcc install-local

install: ${DESTDIR}
	${INSTALL} libmsvcrt.a ${DESTDIR}${ULIBDIR}/
	@${MAKE} -C gcc install

clean:
	/bin/rm -f *.a
	@${MAKE} -C gcc clean
