include ../../Makefile.inc

FILES= man.conf rc.conf rc.local syslog.conf inetd.conf rc netstart motd gettytab \
	login.conf sysctl.conf ftpchroot ftpusers
ETCPATH= ${SRCDIR}/etc

all: ${ETCPATH} ${FILES} ttys MAKEDEV

${ETCPATH}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'etc' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

${DESTDIR}${ETCDIR}:
	@mkdir -p ${DESTDIR}${ETCDIR}
	${INSTALL} ${FILES} ttys MAKEDEV ${DESTDIR}${ETCDIR}/
	cp -R ${SRCDIR}/etc/rc.d ${DESTDIR}${ETCDIR}/

%: ${SRCDIR}/etc/%
	@/bin/cp -uv $< $@

login.conf: ${SRCDIR}/etc/etc.${ARCH}/login.conf
	@/bin/cp -uv $< $@

install: ${DESTDIR}${ETCDIR}

clean:
	/bin/rm -f ${FILES}
