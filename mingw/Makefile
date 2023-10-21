include ../Makefile.inc

UBINDIR=${PREFIX}/bin
LIBEXEC=${PREFIX}/libexec

LIBS=libadvapi32.a libkernel32.a libshell32.a libuser32.a \
	libmoldname.a libstdc++.a libgcc_s_dw2-1.a libgmon.a

all: ${LIBS}
	@${MAKE} -C bin

#
# below -k option makes ld remove "@" suffix
#

%.a: %.def
	dlltool -d $< -l $@ -k

%.exe: ${MINGW}/bin/%.exe
	/bin/cp -vu $< ${UBINDIR}/

%.dll: ${MINGW}/bin/%.dll
	/bin/cp -vu $< ${BINDIR}/

bin.d:
	@${MAKE} -C bin

includes-local:
	@/bin/cp -rvu include/* ${INCDIR}/

${BINDIR}:
	mkdir -p ${BINDIR}

${UBINDIR}:
	mkdir -p ${UBINDIR}

${LIBDIR}:
	mkdir -p ${LIBDIR}

${LIBEXEC}:
	mkdir -p ${LIBEXEC}

GCCBIN=gcc.exe cc.exe cpp.exe
GCCLIB=libgmp-10.dll libmpc-3.dll libmpfr-4.dll zlib1.dll libgcc_s_dw2-1.dll
GCCBIN+=g++.exe c++.exe gdb.exe
GCCLIB+=libintl-8.dll libiconv-2.dll libstdc++-6.dll

#cp /c/MinGW/mingw32/lib/gcc/mingw32/4.8.1/libstdc++.dll.a /lib/gcc/mingw32/4.8.1/libstdc++.a

BINUTILS=ld.exe ar.exe as.exe nm.exe objdump.exe ranlib.exe size.exe strip.exe \
	dlltool.exe gprof.exe addr2line.exe dllwrap.exe elfedit.exe objcopy.exe \
	strings.exe windmc.exe windres.exe c++filt.exe

MSGBIN=msgfmt.exe libgettextsrc-0-18-3.dll libgettextlib-0-18-3.dll

install-gcc: ${BINDIR} ${UBINDIR} ${GCCBIN} ${GCCLIB}
install-ld: ${UBINDIR} ${BINUTILS}

# create dummy ld.so for configure scripts to detect shared libs

install-lib: ${LIBDIR}
	/bin/cp ${LIBS} ${LIBDIR}/

install-libexec: ${LIBEXEC}
	/bin/cp -rvu ${MINGW}/libexec/gcc ${LIBEXEC}/

install-tools: ${MSGBIN}

install-local: install-lib

install:
	@${MAKE} -C gcc install
	@${MAKE} -C bin install

clean:
	/bin/rm -f *.a
	@${MAKE} -C gcc clean

