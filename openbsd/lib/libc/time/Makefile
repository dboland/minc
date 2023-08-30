OBJECTS=asctime.o difftime.o localtime.o strftime.o strptime.o wcsftime.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/time/%.c
	gcc -c ${CFLAGS} -I${SRCDIR}/time -o $@ $<

