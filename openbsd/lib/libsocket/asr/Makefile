OBJECTS= asr.c.o asr_debug.o asr_utils.o \
	getrrsetbyname_async.o \
	gethostnamadr_async.o getnetnamadr_async.o \
	getaddrinfo_async.o getnameinfo_async.o \
	res_send_async.o res_search_async.o res_query.o

OBJECTS+= getaddrinfo.o gethostnamadr.o getnameinfo.o getnetnamadr.o \
	getrrsetbyname.o res_debug.o res_init.c.o res_mkquery.o \
	sethostent.o

all: ${OBJECTS}

%.o: ${SRCDIR}/libc/asr/%.c
	gcc -c ${CFLAGS} -DASR_OPT_HOSTALIASES -o $@ $<

%.c.o: %.c
	gcc -c ${CFLAGS} -I${SRCDIR}/libc/asr -DASR_OPT_HOSTALIASES -o $@ $<

