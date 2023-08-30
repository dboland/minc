OBJECTS= assert.o \
	basename.o clock.o clock_getcpuclockid.o \
	confstr.o ctermid.o ctype_.o \
	dirname.o err.o fnmatch.o \
	errc.o errx.o errlist.o \
	frexp.o fdatasync.o statvfs.o \
	fstab.o ftok.o fts.c.o ftw.o getbsize.o \
	getttyent.o getusershell.o glob.o \
	isctype.o isfdtype.o isfinite.o isinf.o \
	isnan.o isnormal.o signbit.o lockf.o \
	nftw.o pause.o popen.o psignal.o \
	pw_dup.o readpassphrase.o scandir.o \
	setjmperr.o setmode.o \
	siginterrupt.o siglist.o signal.o signame.o \
	sigsetops.o sleep.o strtofflags.o \
	time.o times.o timezone.o getlogin.o \
	tolower_.o vis.o unvis.o \
	ttyslot.o toupper_.o usleep.o \
	utime.o valloc.o wait.o wait3.o waitpid.o \
	warn.o warnc.o warnx.o \
	pwcache.o daemon.o fpclassify.o getcap.o getmntinfo.o \
	nice.o alarm.o sysctl.o getcwd.o devname.o sysconf.o \
	getpagesize.o getloadavg.o getdomainname.o setdomainname.o \
	gethostname.o sethostname.o uname.o isatty.o initgroups.o disklabel.o \
	getnetgrent.o ttyname.o ualarm.o raise.o nlist.o

OBJECTS+= getpwent.c.o getgrent.c.o setproctitle.c.o

#OBJECTS+= vwarn.c.o vwarnc.c.o vwarnx.c.o verr.c.o verrc.c.o verrx.c.o \
#	setprogname.c.o getprogname.c.o exec.c.o

OBJECTS+= vwarn.o vwarnc.o vwarnx.o verr.o verrc.o verrx.o \
	setprogname.c.o getprogname.c.o exec.o

OBJECTS+= opendir.o readdir.o readdir_r.o rewinddir.o seekdir.o dirfd.o telldir.o closedir.o 

#OBJECTS+= elf_hash.o posix_spawn.o shm_open.o

#LIBSOCKET= syslog_r.c.o syslog.o auth_subr.o login_cap.o authenticate.o

#REPLACED+= getgrouplist.o errno.o getgrent.o getpwent.o

#_NLIST_DO_ELF

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/gen/%.c
	gcc -c ${CFLAGS} -I${SRCDIR}/time -o $@ $<

%.c.o: %.c
	gcc -c ${CFLAGS} -I../../../../include -o $@ $<

sysconf.o: ${SRCDIR}/gen/sysconf.c
	gcc -c ${CFLAGS} -I${SRCDIR}/../../sys -I../../../sys -o $@ $<

getloadavg.o: getloadavg.c
	gcc -c ${CFLAGS} -I${SRCDIR}/../../sys -o $@ $<

disklabel.o: ${SRCDIR}/gen/disklabel.c
	gcc -c ${CFLAGS} -I${SRCDIR}/../../sys -o $@ $<

clean:
	rm -f *.o *.dll *.a

