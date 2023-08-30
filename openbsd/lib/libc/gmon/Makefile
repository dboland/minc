OBJECTS=gmon.o mcount.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/gmon/%.c
	gcc -c ${CFLAGS} -o $@ $<
