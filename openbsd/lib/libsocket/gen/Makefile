OBJECTS=syslog_r.c.o syslog.o login_cap.o auth_subr.o authenticate.o

SHOBJECTS=syslog_r.c.so syslog.so login_cap.so auth_subr.so authenticate.so

all: ${OBJECTS} ${SHOBJECTS}

%.o: ${SRCDIR}/libc/gen/%.c
	gcc -c ${CFLAGS} -DNOPIE -o $@ $<
%.c.o: %.c
	gcc -c ${CFLAGS} -DNOPIE -o $@ $<

%.so: ${SRCDIR}/libc/gen/%.c
	gcc -c ${CFLAGS} -o $@ $<
%.c.so: %.c
	gcc -c ${CFLAGS} -o $@ $<

