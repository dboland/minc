include ../../Makefile.inc

OBJECTS= dev_mkdb pwd_mkdb
CFLAGS+= -I${SRCDIR}/sys
USBINPATH= ${SRCDIR}/usr.sbin

all: ${USBINPATH} ${OBJECTS}

${USBINPATH}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'usr.sbin' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

dev_mkdb: ${SRCDIR}/usr.sbin/dev_mkdb/dev_mkdb.c
	gcc -s ${CFLAGS} -o $@ $<

pwd_mkdb: ${SRCDIR}/usr.sbin/pwd_mkdb/pwd_mkdb.c
	gcc -s ${CFLAGS} -o $@ $< -lutil

install: ${DESTDIR}
	@mkdir -p ${DESTDIR}/usr/sbin
	${INSTALL} ${OBJECTS} ${DESTDIR}/usr/sbin/

clean:
	/bin/rm -f *.exe *.o
