OBJECTS=crypt.o crypt2.o cryptutil.o blowfish.o bcrypt.o arc4random.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/crypt/%.c
	gcc -c ${CFLAGS} -o $@ $<

