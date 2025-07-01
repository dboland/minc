include ../../Makefile.inc

FILES= man.conf motd gettytab login.conf sysctl.conf ftpchroot ftpusers
ETCSRC= ${SRCDIR}/etc

all: ${ETCSRC} ${FILES} ttys MAKEDEV

${ETCSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'etc' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

${DESTDIR}${ETCDIR}: ${FILES}
	@mkdir -p ${DESTDIR}${ETCDIR}
	${INSTALL} ${FILES} ttys MAKEDEV ${DESTDIR}${ETCDIR}/

%: ${ETCSRC}/%
	@/bin/cp -uv $< $@

login.conf:
	@/bin/cp -uv ${SRCDIR}/etc/etc.${ARCH}/login.conf $@

install: ${DESTDIR}${ETCDIR}
