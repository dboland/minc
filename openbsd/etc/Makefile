include ../../Makefile.inc

FILES= fstab man.conf rc.conf rc.local syslog.conf inetd.conf rc netstart motd gettytab \
	login.conf sysctl.conf ftpchroot ftpusers

all: ${FILES} ttys

${DESTDIR}${ETCDIR}:
	@mkdir -p ${DESTDIR}${ETCDIR}
	${INSTALL} ${FILES} ${DESTDIR}${ETCDIR}/
	${INSTALL} ttys ${DESTDIR}${ETCDIR}/
	cp -R ${SRCDIR}/etc/rc.d ${DESTDIR}${ETCDIR}/

%: ${SRCDIR}/etc/%
	@/bin/cp -uv $< $@

%.dir: ${SRCDIR}/etc/%
	@/bin/cp -uvr $< ${DESTDIR}${ETCDIR}/

fstab:
	mkent fstab >fstab

login.conf: ${SRCDIR}/etc/etc.${ARCH}/login.conf
	@/bin/cp -uv $< $@

install: ${DESTDIR}${ETCDIR}

clean:
	/bin/rm -f ${FILES}
