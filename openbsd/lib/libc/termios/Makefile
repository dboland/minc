OBJECTS= cfgetispeed.o cfgetospeed.o cfmakeraw.o cfsetispeed.o cfsetospeed.o \
	cfsetspeed.o tcdrain.o tcflow.o tcflush.o tcgetattr.o tcgetpgrp.o \
	tcsendbreak.o tcsetattr.o tcsetpgrp.o tcgetsid.o

#OBJECTS+=

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/termios/%.c
	gcc -c ${CFLAGS} -o $@ $<

clean:
	rm -f *.o
