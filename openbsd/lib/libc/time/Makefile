OBJECTS=asctime.o difftime.o localtime.o strftime.o strptime.o wcsftime.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/time/%.c
	${XGCC} -c ${CFLAGS} -I../include -o $@ $<

