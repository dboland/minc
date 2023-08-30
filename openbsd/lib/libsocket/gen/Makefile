OBJECTS=syslog_r.c.o syslog.o login_cap.o auth_subr.o authenticate.o

all: ${OBJECTS}

%.o: ${SRCDIR}/libc/gen/%.c
	gcc -c ${CFLAGS} -o $@ $<

%.c.o: %.c
	gcc -c ${CFLAGS} -o $@ $<

