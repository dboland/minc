OBJECTS = citrus_ctype.o citrus_none.o citrus_utf8.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/citrus/%.c
	gcc -c ${CFLAGS} -o $@ $<

