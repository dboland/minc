include ../../../Makefile.inc

RELEASE=12.1

LIBUTIL=libutil.so.${RELEASE}
IMPLIB=libutil.dll.a

LIBROOT=../../..
GCCLIB=${LIBROOT}/mingw/gcc
POSIXLIB=${LIBROOT}/libposix

OBJECTS=bcrypt_pbkdf.o duid.o \
	login_tty.o logout.o logwtmp.o pty.o \
	uucplock.o fparseln.o opendisk.o pidfile.o \
	fmt_scaled.o pkcs5_pbkdf2.o \
	getrawpartition.o readlabel.o opendev.o
OBJECTS+=ohash_create_entry.o ohash_delete.o ohash_do.o ohash_entries.o \
	ohash_enum.o ohash_init.o ohash_interval.o \
	ohash_lookup_interval.o ohash_lookup_memory.o \
	ohash_qlookup.o ohash_qlookupi.o
OBJECTS+= getmaxpartitions.o
# BSD Auth
OBJECTS+=passwd.o check_expire.o login_fbtab.o imsg.o imsg-buffer.o login.o

HEADERS=util.h imsg.h

all: libutil.a

%.o: ${SRCDIR}/lib/libutil/%.c
	gcc -c ${CFLAGS} -I../../include -o $@ $<

pidfile.o: pidfile.c
	gcc -c ${CFLAGS} -I../../include -o $@ $<

libutil.a: ${OBJECTS}
	ar cru $@ ${OBJECTS}

${LIBUTIL}: ${OBJECTS}
	${LD} -shared -s -o $@ -e _DllMainCRTStartup@12 --disable-auto-import \
	--out-implib=${IMPLIB} ${GCCLIB}/crt1.o ${OBJECTS} \
	-L/lib -L${GCCLIB} -lmsvcrt -lmingw32 -lmingwex -lsocket -lgcc

clean:
	/bin/rm -f *.o *.a ${LIBUTIL}

%.h: ${SRCDIR}/lib/libutil/%.h
	@cp -vu $< ${DESTDIR}${INCDIR}/$@

install-local: ${HEADERS}
	/bin/cp libutil.a ${LIBDIR}/
