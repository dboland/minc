include ../Makefile.inc

LIBEXEC=${PREFIX}/libexec
LIBS=libadvapi32.a libkernel32.a libshell32.a libuser32.a \
	libmoldname.a libgmon.a

all: ${MINGW} ${LIBS} libiphlpapi.a
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

%.exe: ${MINGW}/bin/%.exe
	@/bin/cp -vu $< ${UBINDIR}/

%.dll: ${MINGW}/bin/%.dll
	@/bin/cp -vu $< ${BINDIR}/

${BINDIR}:
	/bin/mkdir -p ${BINDIR}

${UBINDIR}:
	/bin/mkdir -p ${UBINDIR}

${LIBDIR}:
	/bin/mkdir -p ${LIBDIR}

${LIBEXEC}:
	/bin/mkdir -p ${LIBEXEC}
	/bin/cp -rvu ${MINGW}/libexec/gcc ${LIBEXEC}/

GCCBIN+=gcc.exe cpp.exe
GCCLIB+=libgmp-10.dll libmpc-3.dll libmpfr-4.dll zlib1.dll libgcc_s_dw2-1.dll
GCCBIN+=g++.exe c++.exe gdb.exe
GCCLIB+=libintl-8.dll libiconv-2.dll libstdc++-6.dll

BINUTILS=ld.exe ar.exe as.exe nm.exe objdump.exe ranlib.exe size.exe strip.exe \
	dlltool.exe gprof.exe addr2line.exe dllwrap.exe elfedit.exe objcopy.exe \
	strings.exe windmc.exe windres.exe c++filt.exe

MSGBIN=msgfmt.exe libgettextsrc-0-18-3.dll libgettextlib-0-18-3.dll

install-gcc: ${BINDIR} ${UBINDIR} ${GCCBIN} ${GCCLIB}
install-ld: ${UBINDIR} ${BINUTILS}
install-tools: ${MSGBIN}

# tip: create dummy ld.so for configure scripts to detect shared libs

install-local: ${MINGW} install-gcc install-ld ${LIBEXEC} ${LIBDIR}
	/bin/cp -vu ${LIBS} ${LIBDIR}/
	/bin/cp -vu libiphlpapi.a ${MINGW}/lib/libiphlpapi.dll.a
	@${MAKE} -C gcc install-local

install:
	@${MAKE} -C gcc install
	@${MAKE} -C bin install

clean:
	@${MAKE} -C bin clean
	@${MAKE} -C gcc clean
	/bin/rm -f *.a

