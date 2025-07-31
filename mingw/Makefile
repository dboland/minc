ROOTDIR= ..

include ${ROOTDIR}/Makefile.inc

LIBS= libadvapi32.a libkernel32.a libshell32.a libuser32.a \
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

# below -k option makes ld remove "@" suffix (--enable-stdcall-fixup)

%.a: %.def
	dlltool -d $< -l $@ -k

${BINDIR}/%.dll: ${MINGW}/bin/%.dll
	@/bin/cp -vu $^ $@

${BINDIR}/%.exe: ${MINGW}/bin/%.exe
	@/bin/cp -vu $^ $@

${BINDIR}:
	@/bin/mkdir -p $@

${GBINDIR}/%.exe: ${MINGW}/bin/%.exe
	@/bin/cp -vu $^ $@

${GBINDIR}:
	@/bin/mkdir -p $@

${GLIBDIR}:
	@/bin/mkdir -p $@

${GLIBXDIR}:
	@/bin/mkdir -p $@
	/bin/cp -ru ${MINGW}/libexec/gcc $@/

GCCBIN+= ${GBINDIR}/gcc.exe ${GBINDIR}/cpp.exe
GCCBIN+= ${GBINDIR}/g++.exe ${GBINDIR}/c++.exe
#GCCBIN+=  ${GBINDIR}/gdb.exe

GCCLIB+= ${BINDIR}/libgmp-10.dll ${BINDIR}/libmpc-3.dll ${BINDIR}/libmpfr-4.dll \
	${BINDIR}/zlib1.dll ${BINDIR}/libgcc_s_dw2-1.dll
GCCLIB+= ${BINDIR}/libintl-8.dll ${BINDIR}/libiconv-2.dll ${BINDIR}/libstdc++-6.dll

BINUTILS= ${GBINDIR}/windres.exe
BINUTILS+= ${BINDIR}/ld.exe ${BINDIR}/ar.exe ${BINDIR}/as.exe ${BINDIR}/nm.exe \
	${BINDIR}/dlltool.exe
#BINUTILS+= ${BINDIR}/objdump.exe ${BINDIR}/ranlib.exe ${BINDIR}/size.exe \
	${BINDIR}/strip.exe ${BINDIR}/gprof.exe \
	${BINDIR}/addr2line.exe ${BINDIR}/dllwrap.exe ${BINDIR}/elfedit.exe \
	${BINDIR}/objcopy.exe ${BINDIR}/strings.exe ${BINDIR}/windmc.exe \
	${BINDIR}/c++filt.exe

#MSGBIN= ${BINDIR}/msgfmt.exe ${BINDIR}/libgettextsrc-0-18-3.dll \
	${BINDIR}/libgettextlib-0-18-3.dll

install-gcc: ${GBINDIR} ${GCCBIN} ${BINDIR} ${GCCLIB}
install-ld: ${GBINDIR} ${BINUTILS}
install-tools: ${MSGBIN}

# tip: create dummy /libexec/ld.so for configure scripts to detect shared libs
# GetIpStatisticsEx() is missing in vanilla MinGW (libiphlpapi.dll.a)

install-local: ${MINGW} install-gcc install-ld ${GLIBXDIR} ${GLIBDIR}
	/bin/cp -vu ${LIBS} ${GLIBDIR}/
	/bin/cp -vu libiphlpapi.dll.a ${MINGW}/lib/
	@${MAKE} -C gcc install-local

install-cross:
	/bin/cp -vu libmsvcrt.a ${XLIBDIR}/
	@${MAKE} -C gcc install-cross

install: ${DESTDIR}
	${INSTALL} libmsvcrt.a ${DESTDIR}${ULIBDIR}/
	@${MAKE} -C gcc install

clean:
	/bin/rm -f *.a
	@${MAKE} -C gcc clean
